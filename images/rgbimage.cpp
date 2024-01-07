#include <algorithm>
#include <cstring>
#include "rgbimage.h"
#include "i444image.h"

RgbImage::RgbImage(int width, int height, bool isInterlaced)
    :ImageBase(width, height, ImgFormat::RGB, isInterlaced)
{
    m_sizeInBytes = m_width*m_height*3;
    m_pixels.resize(m_sizeInBytes);
}

RgbImage::~RgbImage(){
    m_pixels.clear();
}

RgbImage::RgbImage(const RgbImage& other) :
    ImageBase(other.m_width, other.m_height, other.m_format, other.m_isInterlaced)
{
    m_sizeInBytes = other.m_sizeInBytes;
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

std::unique_ptr<I444Image>
RgbImage::convertToI444() const {
    auto i444image = std::make_unique<I444Image>(m_width, m_height, m_isInterlaced);
    convertToI444(i444image);
    return i444image;
}

void
RgbImage::convertToI444(const std::unique_ptr<I444Image> &dst) const {
    if (!dst || dst->getWidth() != m_width || dst->getHeight() != m_height || dst->getFormat() != ImgFormat::I444) {
        throw std::runtime_error("RGB to I444 conversion: invalid I444 destination");
    }
    const uint8_t *buffer = m_pixels.data();

    int offset = m_width*m_height;
    uint8_t *i444BufferY = dst->getBuffer();
    uint8_t *i444BufferU = i444BufferY + offset;
    uint8_t *i444BufferV = i444BufferU + offset;

    for (int i = 0; i < m_height; ++i) {
        for (int j = 0; j < m_width; ++j) {
            i444BufferY[j] = buffer[3*j];
            i444BufferU[j] = buffer[3*j+1];
            i444BufferV[j] = buffer[3*j+2];
        }
        buffer += 3*m_width;
        i444BufferY += m_width;
        i444BufferU += m_width;
        i444BufferV += m_width;
    }
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

void
RgbImage::resize(int width, int height) {    
    m_width = width;
    m_height = height;
    m_sizeInBytes = m_width*m_height*3;
    m_pixels.resize(m_sizeInBytes);
}

