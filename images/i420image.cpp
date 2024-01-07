#include <algorithm>
#include "i420image.h"
#include "rgbimage.h"
#include "i444image.h"

#include <QDebug>

I420Image::I420Image(int width, int height, bool isInterlaced)
    : ImageBase(width, height, ImgFormat::I420, isInterlaced)
{
    m_sizeInBytes = m_width*m_height*3/2;
    m_pixels.resize(m_sizeInBytes);
}

uint8_t*
I420Image::getBuffer() {
    return &m_pixels[0];
}

const uint8_t*
I420Image::getBuffer() const {
    return m_pixels.data();
}

std::unique_ptr<RgbImage>
I420Image::convertToRgb() const {
    auto rgbimage = std::make_unique<RgbImage>(m_width, m_height, m_isInterlaced);
    convertToRgb(rgbimage);
    return rgbimage;
}

void
I420Image::convertToRgb(const std::unique_ptr<RgbImage> &dst) const {
    if (!dst || dst->getWidth() != m_width || dst->getHeight() != m_height || dst->getFormat() != ImgFormat::RGB) {
        throw std::runtime_error("I420 to RGB conversion: invalid RGB destination");
    }
    const uint8_t *bufferY = m_pixels.data();
    int u_offset = m_width*m_height;
    const uint8_t *bufferU = bufferY + u_offset;
    int v_offset = u_offset + (m_width/2)*(m_height/2);
    const uint8_t *bufferV = bufferY + v_offset;

    uint8_t *rgbBuffer = dst->getBuffer();

    if (!m_isInterlaced) {
        for (int i = 0; i < m_height; ++i) {
            //qDebug() << "line i " << i << "\n";
            for (int j = 0; j < m_width; ++j) {
                //qDebug() << "row j " << j << "\n";
                int Y = bufferY[j];
                int U = bufferU[j/2];
                int V = bufferV[j/2];
                int R = 0, G = 0, B = 0;
                ImageBase::RGBfromYUV(R, G, B, Y, U, V);
                rgbBuffer[3*j] = R;
                rgbBuffer[3*j+1] = G;
                rgbBuffer[3*j+2] = B;
            }
            bufferY += m_width;
            if (i%2==1) {
                bufferU += (m_width/2);
                bufferV += (m_width/2);
            }
            rgbBuffer += 3*m_width;
        }
    } else { //interlaced case
        const uint8_t *srcU = bufferU;
        const uint8_t *srcV = bufferV;
        for (int i = 0; i < m_height; ++i) {
            bufferU = (i%2 == 0)? srcU + ((i/4)*2)*(m_width/2) : srcU + ((i/4)*2 +1)*(m_width/2);
            bufferV = (i%2 == 0)? srcV + ((i/4)*2)*(m_width/2) : srcV + ((i/4)*2 +1)*(m_width/2);
            for (int j = 0; j < m_width; ++j) {
                int Y = bufferY[j];
                int U = bufferU[j/2];
                int V = bufferV[j/2];
                int R = 0, G = 0, B = 0;
                ImageBase::RGBfromYUV(R, G, B, Y, U, V);
                rgbBuffer[3*j] = R;
                rgbBuffer[3*j+1] = G;
                rgbBuffer[3*j+2] = B;
            }
            bufferY += m_width;
            rgbBuffer += 3*m_width;
        }
    }
}

std::unique_ptr<I444Image>
I420Image::convertToI444() const {
    auto i444image = std::make_unique<I444Image>(m_width, m_height, m_isInterlaced);
    convertToI444(i444image);
    return i444image;
}

void
I420Image::convertToI444(const std::unique_ptr<I444Image>& dst) const {
    if (!dst || dst->getWidth() != m_width || dst->getHeight() != m_height || dst->getFormat() != ImgFormat::I444) {
        std::string errorMsg = "Invalid I420 to I444 conversion: ";
        errorMsg += " width " + std::to_string(dst->getWidth());
        errorMsg += " height " + std::to_string(dst->getHeight());
        errorMsg += " format " + yuvp::toString(dst->getFormat());
        errorMsg += " interlaced " + std::to_string((int)dst->isInterlaced());
        throw std::runtime_error(errorMsg);
    }
    const uint8_t *bufferY = m_pixels.data();
    int u_offset = m_width*m_height;
    const uint8_t *bufferU = bufferY + u_offset;
    int v_offset = u_offset + (m_width/2)*(m_height/2);
    const uint8_t *bufferV = bufferY + v_offset;

    uint8_t *i444BufferY = dst->getBuffer();
    uint8_t *i444BufferU = i444BufferY + m_width*m_height;
    uint8_t *i444BufferV = i444BufferU + m_width*m_height;

    if (!m_isInterlaced) {
        for (int i = 0; i < m_height; ++i) {
            for (int j = 0; j < m_width; ++j) {
                i444BufferY[j] = bufferY[j];
                i444BufferU[j] = bufferU[j/2];
                i444BufferV[j] = bufferV[j/2];
            }
            bufferY += m_width;
            if (i%2==1) {
                bufferU += (m_width/2);
                bufferV += (m_width/2);
            }
            i444BufferY += m_width;
            i444BufferU += m_width;
            i444BufferV += m_width;
        }
    } else { //interlaced case
        const uint8_t *srcU = bufferU;
        const uint8_t *srcV = bufferV;
        for (int i = 0; i < m_height; ++i) {
            bufferU = (i%2 == 0)? srcU + ((i/4)*2)*(m_width/2) : srcU + ((i/4)*2 +1)*(m_width/2);
            bufferV = (i%2 == 0)? srcV + ((i/4)*2)*(m_width/2) : srcV + ((i/4)*2 +1)*(m_width/2);
            for (int j = 0; j < m_width; ++j) {
                i444BufferY[j] = bufferY[j];
                i444BufferU[j] = bufferU[j/2];
                i444BufferV[j] = bufferV[j/2];
            }
            bufferY += m_width;
            i444BufferY += m_width;
            i444BufferU += m_width;
            i444BufferV += m_width;
        }
    }
}

bool
I420Image::readImage(FileDescriptor &file, int index) {
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
I420Image::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_sizeInBytes = m_width*m_height*3/2;
    m_pixels.resize(m_sizeInBytes);
}
