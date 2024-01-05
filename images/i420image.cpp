#include <algorithm>
#include "i420image.h"
#include "rgbimage.h"

#include <QDebug>

I420Image::I420Image(int width, int height, bool isInterlaced)
    : IImage(width, height, ImgFormat::I420, isInterlaced)
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
                IImage::RGBfromYUV(R, G, B, Y, U, V);
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
                IImage::RGBfromYUV(R, G, B, Y, U, V);
                rgbBuffer[3*j] = R;
                rgbBuffer[3*j+1] = G;
                rgbBuffer[3*j+2] = B;
            }
            bufferY += m_width;
            rgbBuffer += 3*m_width;
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

size_t
I420Image::getSizeInBytes() const {
    return m_sizeInBytes;
}

std::unique_ptr<IImage>
I420Image::clone() const {
    auto ret = std::make_unique<I420Image>(m_width, m_height, m_isInterlaced);
    clone(*ret);
    return ret;
}

void
I420Image::clone(IImage &dst) const {
    if (dst.getWidth() != m_width || dst.getHeight() != m_height || dst.getFormat() != ImgFormat::I420) {
        throw std::runtime_error("Invalid format to clone I420Image");
    }
    std::memcpy(dst.getBuffer(), &m_pixels[0], getSizeInBytes());
}

void
I420Image::resize(int width, int height) {
    m_width = width;
    m_height = height;
    m_sizeInBytes = m_width*m_height*3/2;
    m_pixels.resize(m_sizeInBytes);
}
