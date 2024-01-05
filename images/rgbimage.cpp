#include <algorithm>
#include <cstring>
#include "rgbimage.h"

RgbImage::RgbImage(int width, int height, bool isInterlaced)
    :IImage(width, height, ImgFormat::RGB, isInterlaced)
{
    m_sizeInBytes = m_width*m_height*3;
    m_pixels.resize(m_sizeInBytes);
}

RgbImage::~RgbImage(){
    m_pixels.clear();
}

RgbImage::RgbImage(const RgbImage& other) :
    IImage(other.m_width, other.m_height, other.m_format, other.m_isInterlaced),
    m_sizeInBytes(other.m_sizeInBytes)
{
    m_pixels.resize(m_sizeInBytes);
    std::memcpy(&m_pixels[0], &other.m_pixels[0], m_sizeInBytes);
}

RgbImage& RgbImage::operator=(const RgbImage& other) {
    if (this != &other) {
        m_width = other.m_width;
        m_height = other.m_height;
        m_format = other.m_format;
        m_isInterlaced = other.m_isInterlaced;
        m_sizeInBytes = other.m_sizeInBytes;
        m_pixels.resize(m_sizeInBytes);
        std::memcpy(&m_pixels[0], &other.m_pixels[0], m_sizeInBytes);
    }
    return *this;
}

uint8_t*
RgbImage::getBuffer() {
    return &m_pixels[0];
}

const uint8_t*
RgbImage::getBuffer() const {
    return m_pixels.data();
}

std::unique_ptr<RgbImage>
RgbImage::convertToRgb() const {
    auto dst = std::make_unique<RgbImage>(m_width, m_height, m_isInterlaced);
    clone(*dst);
    return dst;
}

void
RgbImage::convertToRgb(const std::unique_ptr<RgbImage> &dst) const {
    clone(*dst);
}

bool
RgbImage::readImage(FileDescriptor &file, int index) {
    if (file.getIfstream().is_open()) {
        auto &ifstream = file.getIfstream();
        if (index >= 0) {
            int totalNumFrames = file.getSize()/m_sizeInBytes;
            index = std::clamp(index, 0, totalNumFrames-1);
            size_t offset = index * m_sizeInBytes;
            int curPos = -1; //ifstream.tellg();
            if (curPos != -1) {
                ifstream.seekg(offset-curPos, ifstream.cur);
            } else {
                ifstream.seekg(offset, ifstream.beg);
            }
        }
        ifstream.read(reinterpret_cast<char *>(m_pixels.data()), m_sizeInBytes);
        return true;
    }
    return false;
}

size_t
RgbImage::getSizeInBytes() const {
    return m_sizeInBytes;
}

std::unique_ptr<IImage>
RgbImage::clone() const {
    auto ret = std::make_unique<RgbImage>(m_width, m_height, m_isInterlaced);
    clone(*ret);
    return ret;
}

void
RgbImage::clone(IImage &dst) const {
    if (dst.getWidth() != m_width || dst.getHeight() != m_height || dst.getFormat() != ImgFormat::RGB || dst.isInterlaced() != m_isInterlaced) {
        std::string errorMsg = "Invalid format to clone RgbImage";
        errorMsg += " width " + std::to_string(dst.getWidth());
        errorMsg += " height " + std::to_string(dst.getHeight());
        errorMsg += " format " + std::to_string((int)dst.getFormat());
        errorMsg += " interlaced " + std::to_string((int)dst.isInterlaced());
        throw std::runtime_error(errorMsg);
    }
    if (dst.getWidth() != m_width || dst.getHeight() != m_height) {
        dst.resize(m_width, m_height);
    }
    std::memcpy(dst.getBuffer(), &m_pixels[0], getSizeInBytes());
}

void
RgbImage::resize(int width, int height) {
    if (width != m_width || height != m_height) {
        m_width = width;
        m_height = height;
        m_sizeInBytes = m_width*m_height*3;
        m_pixels.resize(m_sizeInBytes);
    }
}

