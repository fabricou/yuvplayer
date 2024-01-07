#include <algorithm>
#include "i444image.h"
#include "rgbimage.h"

I444Image::I444Image(int width, int height, bool isInterlaced)
    : ImageBase(width, height, ImgFormat::I444, isInterlaced)
{
    m_sizeInBytes = m_width*m_height*3;
    m_pixels.resize(m_sizeInBytes);
}

uint8_t*
I444Image::getBuffer() {
    return &m_pixels[0];
}

const uint8_t*
I444Image::getBuffer() const {
    return m_pixels.data();
}

std::unique_ptr<RgbImage>
I444Image::convertToRgb() const {
    auto rgbimage = std::make_unique<RgbImage>(m_width, m_height, m_isInterlaced);
    convertToRgb(rgbimage);
    return rgbimage;
}

void
I444Image::convertToRgb(const std::unique_ptr<RgbImage> &dst) const {
    if (!dst || dst->getWidth() != m_width || dst->getHeight() != m_height || dst->getFormat() != ImgFormat::RGB) {
        throw std::runtime_error("I444 to RGB conversion: invalid RGB destination");
    }
    int offset = m_width*m_height;
    const uint8_t *bufferY = m_pixels.data();
    const uint8_t *bufferU = bufferY + offset;
    const uint8_t *bufferV = bufferU + offset;

    uint8_t *rgbBuffer = dst->getBuffer();

    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            int Y = bufferY[j];
            int U = bufferU[j];
            int V = bufferV[j];
            int R = 0, G = 0, B = 0;
            ImageBase::RGBfromYUV(R, G, B, Y, U, V);
            rgbBuffer[3*j] = R;
            rgbBuffer[3*j+1] = G;
            rgbBuffer[3*j+2] = B;
        }
        bufferY += m_width;
        bufferU += m_width;
        bufferV += m_width;
        rgbBuffer += 3*m_width;
    }
}

std::unique_ptr<I444Image>
I444Image::convertToI444() const {
    auto dst = std::make_unique<I444Image>(m_width, m_height, m_isInterlaced);
    clone(*dst);
    return dst;
}

void
I444Image::convertToI444(const std::unique_ptr<I444Image> &dst) const {
    clone(*dst);
}

bool
I444Image::readImage(FileDescriptor &file, int index) {
    if (file.getIfstream().is_open()) {
        auto &ifstream = file.getIfstream();
        if (index >= 0) {
            int totalNumFrames = file.getSize()/m_sizeInBytes;
            index = std::clamp(index, 0, totalNumFrames-1);
            size_t offset = index * m_sizeInBytes;
            ifstream.seekg(offset, ifstream.beg);
        }
        ifstream.read(reinterpret_cast<char *>(m_pixels.data()), m_sizeInBytes);
        return true;
    }
    return false;
}

void
I444Image::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_sizeInBytes = m_width*m_height*3;
    m_pixels.resize(m_sizeInBytes);
}
