#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

#include <memory>
#include <atomic>
#include <functional>
#include <limits>
#include <mutex>
#include <thread>
#include "../fileDescriptor/filedescriptor.h"
#include "../images/imgformat.h"
#include "../images/rgbimage.h"
#include "../imageProcessor/imageprocessor.h"
#include "../utils/zoom.h"
#include "../utils/colorMode.h"
#include "../utils/grid.h"
#include "../utils/magnifier.h"

class PlayerController
{
public:
    PlayerController(std::function<void(void)> createScreen,
        std::function<void(void)> deleteScreen,
        std::function<void(const RgbImage&,int,int)> displayImage);
    ~PlayerController();

    PlayerController(const PlayerController&) = delete;
    PlayerController& operator=(const PlayerController&) = delete;

    void openYuvFile(const std::string &filename);
    void setWidth(int width);
    void setHeight(int height);
    void setImageFormat(ImgFormat format);
    void setImageNumber(size_t imageNum);
    void computeAutoImageSize(ImgFormat format, int &width, int &height) const;
    void setInterlacedMode(bool isInterlaced);
    void setZoom(Zoom zoom);
    void setColorMode(ColorMode colorMode);
    void setGrid(Grid grid);
    void setMagnifier(Magnifier magnifier);

    //player control
    void displayFirstFrame();
    void displayPreviousFrame();
    void displayNextFrame();
    void displayLastFrame();
    void playBackward();
    void pause();
    void playForward();

private:
    void refresh(bool resetFrameIndex = false);

    //delegated to extern API
    void createScreen();
    void deleteScreen();
    void displayImage();

    //player management
    void stopPlayerThread();
    void startPlayerThread(std::function<void(std::atomic<bool> &)> playFunc);
    void createImages();

    std::unique_ptr<FileDescriptor> m_file;
    std::unique_ptr<ImageBase> m_image{nullptr};
    std::unique_ptr<RgbImage> m_rgbImage{nullptr};
    std::unique_ptr<I444Image> m_i444Image{nullptr};
    std::unique_ptr<RgbImage> m_rgbDisplayImage{nullptr};

    size_t m_imageNum{std::numeric_limits<size_t>::max()};
    size_t m_numberOfImages{0};
    int m_width{-1};
    int m_height{-1};
    bool m_isInterlaced{false};
    ImgFormat m_format{ImgFormat::UNDEF};
    Zoom m_zoom{Zoom::UNDEF};
    ColorMode m_colorMode{ColorMode::UNDEF};
    Grid m_grid{Grid::UNDEF};
    Magnifier m_magnifier;

    std::function<void(void)> m_createScreen = [](){};
    std::function<void(void)> m_deleteScreen = [](){};
    std::function<void(const RgbImage&,int,int)> m_displayImage = [](const RgbImage&,int,int){};

    std::mutex m_mutex;
    std::thread m_playerThread;
    std::atomic<bool> m_stopPlayer{false};
};

#endif // PLAYERCONTROLLER_H
