#include <algorithm>
#include <array>
#include <cstring>
#include "imagebase.h"
#include "i420image.h"
#include "i444image.h"
#include "rgbimage.h"

static void RGBfromYUV(int& R, int& G, int& B, int Y, int U, int V)
{
    Y -= 16;
    U -= 128;
    V -= 128;
    R = std::clamp((int)(1.164 * Y             + 1.596 * V), 0, 255);
    G = std::clamp((int)(1.164 * Y - 0.392 * U - 0.813 * V), 0, 255);
    B = std::clamp((int)(1.164 * Y + 2.017 * U), 0, 255);
}

struct _RGB {
    int R;
    int G;
    int B;
};

static std::array<std::array<std::array<_RGB, 256>, 256>, 256> createYuvToRgbLut() {
    std::array<std::array<std::array<_RGB, 256>, 256>, 256> yuvToRgbLut;
    for (int y = 0; y < 256; y++) {
        for (int u = 0; u < 256; u++) {
            for (int v = 0; v < 256; v++) {
                _RGB& rgb = yuvToRgbLut[y][u][v];
                RGBfromYUV(rgb.R, rgb.G, rgb.B, y, u, v);
            }
        }
    }
    return yuvToRgbLut;
}

void
ImageBase::RGBfromYUV(int& R, int& G, int& B, int Y, int U, int V)
{
    static auto yuvToRgbLut = createYuvToRgbLut();
    Y = std::clamp(Y, 0, 255);
    U = std::clamp(U, 0, 255);
    V = std::clamp(V, 0, 255);
    R = yuvToRgbLut[Y][U][V].R;
    G = yuvToRgbLut[Y][U][V].G;
    B = yuvToRgbLut[Y][U][V].B;
}

ImageBase::ImageBase(int width, int height, ImgFormat format, bool isInterlaced)
    : m_width(width),
      m_height(height),
      m_isInterlaced(isInterlaced),
      m_format(format)
{
}

ImageBase::~ImageBase() {
}

std::unique_ptr<ImageBase>
ImageBase::clone() const {
    auto ret = ImageBase::create(m_width, m_height, m_format, m_isInterlaced);
    clone(*ret);
    return ret;
}

void
ImageBase::clone(ImageBase &dst) const {
    if (dst.getWidth() != m_width || dst.getHeight() != m_height || dst.isInterlaced() != m_isInterlaced || dst.getFormat() != m_format) {
        std::string errorMsg = "Invalid format to clone " + yuvp::toString(m_format) + " Image";
        errorMsg += " width " + std::to_string(dst.getWidth());
        errorMsg += " height " + std::to_string(dst.getHeight());
        errorMsg += " format " + std::to_string((int)dst.getFormat());
        errorMsg += " interlaced " + std::to_string((int)dst.isInterlaced());
        throw std::runtime_error(errorMsg);
    }
    std::memcpy(dst.getBuffer(), getBuffer(), getSizeInBytes());
}

std::unique_ptr<ImageBase>
ImageBase::create(int width, int height, ImgFormat format, bool isInterlaced) {
    switch(format) {
    case ImgFormat::I420:
        return std::make_unique<I420Image>(width, height, isInterlaced);
    case ImgFormat::I444:
        return std::make_unique<I444Image>(width, height, isInterlaced);
    case ImgFormat::RGB:
        return std::make_unique<RgbImage>(width, height, isInterlaced);
    default:
        throw std::runtime_error("IImage::create: Format not supported");
    }
}
