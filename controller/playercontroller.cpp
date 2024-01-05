#include <chrono>
#include <ctime>
#include "playercontroller.h"
#include "../images/iimage.h"
#include "../imageProcessor/interlacedimageprocessor.h"
#include "../imageProcessor/zoomimageprocessor.h"
#include "../imageProcessor/colormodeimageprocessor.h"
#include "../imageProcessor/gridimageprocessor.h"
#include "../imageProcessor/magnifierimageprocessor.h"

PlayerController::PlayerController(std::function<void(void)> createScreen,
                 std::function<void(void)> deleteScreen,
                 std::function<bool(void)> isScreenAvailable,
                 std::function<void(const RgbImage&)> displayImage)
    : m_createScreen(createScreen),
    m_deleteScreen(deleteScreen),
    m_isScreenAvailable(isScreenAvailable),
    m_displayImage(displayImage)
{
}

PlayerController::~PlayerController() {
    stopPlayerThread();
}

void
PlayerController::createImages() {
    if (!m_image || m_image->getWidth() != m_width || m_image->getHeight() != m_height || m_image->getFormat() != m_format || m_image->isInterlaced() != m_isInterlaced) {
        m_image = IImage::create(m_width, m_height, m_format, m_isInterlaced);
    }
    if (!m_rgbImage || m_rgbImage->getWidth() != m_width || m_rgbImage->getHeight() != m_height || m_rgbImage->getFormat() != ImgFormat::RGB) {
        m_rgbImage = std::make_unique<RgbImage>(m_width, m_height, m_isInterlaced);
    }
    if (!m_rgbDisplayImage || m_rgbDisplayImage->getFormat() != ImgFormat::RGB) {
        m_rgbDisplayImage = std::make_unique<RgbImage>(m_width, m_height, m_isInterlaced);
    }
}

void
PlayerController::refresh(bool resetFrameIndex) {
    std::lock_guard lock(m_mutex);

    if (m_file && m_file->isOpen() && m_format != ImgFormat::UNDEF && m_width >= 0 && m_height >= 0) {
        if (!isScreenAvailable()) {
            createScreen();
        }
        createImages();
        if (resetFrameIndex) {
            m_imageNum = 0;
        }
        m_numberOfImages = m_file->getSize() / m_image->getSizeInBytes();
        m_image->readImage(*m_file, m_imageNum);
        displayImage();
    }
}

void
PlayerController::openYuvFile(const std::string &filename) {
    if (!m_file || m_file->getFilename() != filename) {
        m_file = std::make_unique<FileDescriptor>(filename);
        refresh(true);
    }
}

void
PlayerController::setWidth(int width) {
    if (width != m_width) {
        m_width = width;
        refresh(true);
    }
}

void
PlayerController::setHeight(int height) {
    if (height != m_height) {
        m_height = height;
        refresh(true);
    }
}

void
PlayerController::setImageFormat(ImgFormat format) {
    if (format != m_format) {
        m_format = format;
        refresh(true);
    }
}

void
PlayerController::setInterlacedMode(bool isInterlaced) {
    if (isInterlaced != m_isInterlaced) {
        m_isInterlaced = isInterlaced;
        refresh(false);
    }
}

void
PlayerController::setZoom(Zoom zoom) {
    if (zoom != m_zoom) {
        m_zoom = zoom;
        refresh(false);
    }
}

void
PlayerController::setColorMode(ColorMode colorMode) {
    if (colorMode != m_colorMode) {
        m_colorMode = colorMode;
        refresh(false);
    }
}

void
PlayerController::setGrid(Grid grid) {
    if (grid != m_grid) {
        m_grid = grid;
        refresh(false);
    }
}

void
PlayerController::setMagnifier(Magnifier magnifier) {
    if (magnifier != m_magnifier) {
        m_magnifier = magnifier;
        refresh(false);
    }
}

void
PlayerController::setImageNumber(size_t imageNum) {
    std::lock_guard lock(m_mutex);
    if (imageNum != m_imageNum) {
        m_imageNum = imageNum;
        if (m_image && m_file) {
            m_image->readImage(*m_file, m_imageNum);
            displayImage();
        }
    }
}

void
PlayerController::computeAutoImageSize(ImgFormat format, int &width, int &height) const {
    static std::pair<int, int> resolutions[] = {
        {1920, 1080},
        {1280, 720},
        {720, 576},
        {720, 480},
        {640, 480},
        {352, 288},
        {320, 240},
        {176, 144}
    };

    width = -1;
    height = -1;
    if (m_file && m_file->isOpen()) {
        auto fileSize = m_file->getSize();
        for (const auto &res : resolutions) {
            int frameSize = res.first * res.second;
            if (format == ImgFormat::I420) {
                frameSize = (frameSize*3)/2;
            } else if (format == ImgFormat::UYVY) {
                frameSize *= 2;
            }
            if (fileSize % frameSize == 0) {
                width = res.first;
                height = res.second;
                return;
            }
        }
    }
}

void
PlayerController::createScreen() {
    m_createScreen();
}

void
PlayerController::deleteScreen() {
    m_deleteScreen();
}

bool
PlayerController::isScreenAvailable() {
    return m_isScreenAvailable();
}

void
PlayerController::displayImage() {
    if (m_image) {
        m_image->convertToRgb(m_rgbImage);
        std::unique_ptr<ImageProcessor> processor = std::make_unique<ImageProcessor>();
        //warning: order of image processing matter
        if (!m_magnifier.getPoints().empty()) {
            for (const auto &point : m_magnifier.getPoints()) {
                std::unique_ptr<ImageProcessor> magnifierProcessor = std::make_unique<MagnifierImageProcessor>(point);
                processor->addImageProcessor(std::move(magnifierProcessor));
            }
        }
        if (m_zoom != Zoom::UNDEF) {
            std::unique_ptr<ImageProcessor> zoomProcessor = std::make_unique<ZoomImageProcessor>(m_zoom);
            processor->addImageProcessor(std::move(zoomProcessor));
        }
        if (m_grid != Grid::UNDEF) {
            std::unique_ptr<ImageProcessor> gridProcessor = std::make_unique<GridImageProcessor>(m_grid);
            processor->addImageProcessor(std::move(gridProcessor));
        }
        if (m_colorMode != ColorMode::UNDEF) {
            std::unique_ptr<ImageProcessor> colorModeProcessor = std::make_unique<ColorModeImageProcessor>(m_colorMode);
            processor->addImageProcessor(std::move(colorModeProcessor));
        }
        if (m_isInterlaced) {
            std::unique_ptr<ImageProcessor> interlacedProcessor = std::make_unique<InterlacedImageProcessor>();
            processor->addImageProcessor(std::move(interlacedProcessor));
        }
        m_rgbDisplayImage->resize(m_rgbImage->getWidth(), m_rgbImage->getHeight());
        m_rgbImage->clone(*m_rgbDisplayImage);
        processor->processImage(*m_rgbDisplayImage);
        m_displayImage(*m_rgbDisplayImage);
    }
}

void
PlayerController::stopPlayerThread() {
    if (m_playerThread.joinable()) {
        m_stopPlayer = true;
        m_playerThread.join();
    }
}

void
PlayerController::startPlayerThread(std::function<void(std::atomic<bool> &)> playFunc) {
    stopPlayerThread();
    if (m_file && m_file->isOpen() && m_format != ImgFormat::UNDEF && m_width >= 0 && m_height >= 0) {
        if (!isScreenAvailable()) {
            deleteScreen();
            createScreen();
        }
        createImages();
        m_stopPlayer = false;
        m_playerThread = std::thread(playFunc, std::ref(m_stopPlayer));
    }
}

void
PlayerController::displayFirstFrame() {
    auto playFunc = [this](std::atomic<bool> &){
        std::lock_guard lock(m_mutex);
        m_imageNum = 0;
        m_image->readImage(*m_file, m_imageNum);
        displayImage();
    };
    stopPlayerThread();
    startPlayerThread(playFunc);
    stopPlayerThread();
}

void
PlayerController::displayPreviousFrame() {
    auto playFunc = [this](std::atomic<bool> &){
        std::lock_guard lock(m_mutex);
        m_imageNum = (m_numberOfImages + m_imageNum-1) % m_numberOfImages;
        m_image->readImage(*m_file, m_imageNum);
        displayImage();
    };
    stopPlayerThread();
    startPlayerThread(playFunc);
    stopPlayerThread();
}

void
PlayerController::displayNextFrame() {
    auto playFunc = [this](std::atomic<bool> &){
        std::lock_guard lock(m_mutex);
        m_imageNum = (m_imageNum+1) % m_numberOfImages;
        m_image->readImage(*m_file, m_imageNum);
        displayImage();
    };
    stopPlayerThread();
    startPlayerThread(playFunc);
    stopPlayerThread();
}

void
PlayerController::displayLastFrame() {
    auto playFunc = [this](std::atomic<bool> &){
        std::lock_guard lock(m_mutex);
        m_imageNum = m_numberOfImages - 1;
        m_image->readImage(*m_file, m_imageNum);
        displayImage();
    };
    stopPlayerThread();
    startPlayerThread(playFunc);
    stopPlayerThread();
}

void
PlayerController::playBackward() {
    auto playFunc = [this](std::atomic<bool> &stopPlayer){
        while(!stopPlayer) {
            auto t_start = std::chrono::high_resolution_clock::now();
            {
            std::lock_guard lock(m_mutex);
            m_imageNum = (m_numberOfImages + m_imageNum-1) % m_numberOfImages;
            m_image->readImage(*m_file, m_imageNum);
            displayImage();
            }
            const auto t_end = std::chrono::high_resolution_clock::now();
            if (auto timeDuration = std::chrono::duration<double, std::milli>(t_end - t_start);
                timeDuration < std::chrono::milliseconds(16)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(16)-timeDuration);
            }
        }
    };
    stopPlayerThread();
    startPlayerThread(playFunc);
}

void
PlayerController::pause() {
    stopPlayerThread();
}

void
PlayerController::playForward() {
    auto playFunc = [this](std::atomic<bool> &stopPlayer){
        while(!stopPlayer) {
            auto t_start = std::chrono::high_resolution_clock::now();
            {
            std::lock_guard lock(m_mutex);
            m_imageNum = (m_imageNum+1) % m_numberOfImages;
            m_image->readImage(*m_file, m_imageNum);
            displayImage();
            }
            const auto t_end = std::chrono::high_resolution_clock::now();
            if (auto timeDuration = std::chrono::duration<double, std::milli>(t_end - t_start);
                timeDuration < std::chrono::milliseconds(16)) {
                std::this_thread::sleep_for(std::chrono::milliseconds(16)-timeDuration);
            }
        }
    };
    stopPlayerThread();
    startPlayerThread(playFunc);
}
