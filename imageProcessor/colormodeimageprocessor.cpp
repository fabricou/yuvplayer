#include "colormodeimageprocessor.h"
#include <algorithm>
#include <array>

static void YUVfromRGB(int &Y, int &U, int &V, int R, int G, int B)
{
    Y = std::clamp((int)(0.257 * R + 0.504 * G + 0.098 * B +  16), 0, 255);
    U = std::clamp((int)(-0.148 * R - 0.291 * G + 0.439 * B + 128), 0, 255);
    V = std::clamp((int)(0.439 * R - 0.368 * G - 0.071 * B + 128), 0, 255);
}

struct _YUV {
    int Y;
    int U;
    int V;
};

static std::array<std::array<std::array<_YUV, 256>, 256>, 256> createRgbToYuvLut() {
    std::array<std::array<std::array<_YUV, 256>, 256>, 256> rgbToYuvLut;
    for (int r = 0; r < 256; r++) {
        for (int g = 0; g < 256; g++) {
            for (int b = 0; b < 256; b++) {
                _YUV& yuv = rgbToYuvLut[r][g][b];
                YUVfromRGB(yuv.Y, yuv.U, yuv.V, r, g, b);
            }
        }
    }
    return rgbToYuvLut;
}

ColorModeImageProcessor::ColorModeImageProcessor(ColorMode ColorMode)
    : ImageProcessor(),
    m_colorMode(ColorMode)
{
}

std::unique_ptr<RgbImage>
ColorModeImageProcessor::processImage(std::unique_ptr<RgbImage> src) const {
    std::unique_ptr<RgbImage> dst = std::make_unique<RgbImage>(src->getWidth(), src->getHeight());
    src->clone(*dst);
    processImage(*dst);
    return dst;
}

void
ColorModeImageProcessor::processImage(RgbImage& src) const {
    static auto rgbToYuvLut = createRgbToYuvLut();

    ImageProcessor::processImage(src);
    if (m_colorMode == ColorMode::UNDEF || m_colorMode == ColorMode::YUV) {
        return;
    }
    auto clone = src.clone();
    uint8_t *srcBuffer = clone->getBuffer();
    uint8_t *dstBuffer = src.getBuffer();
    for (int i = 0; i < src.getWidth()*src.getHeight(); ++i) {

        uint8_t R = srcBuffer[0];
        uint8_t G = srcBuffer[1];
        uint8_t B = srcBuffer[2];

        uint8_t color = 0;
        if (m_colorMode == ColorMode::Y) {
            color = rgbToYuvLut[R][G][B].Y;
        } else if (m_colorMode == ColorMode::U) {
            color = rgbToYuvLut[R][G][B].U;
        } else if (m_colorMode == ColorMode::V) {
            color = rgbToYuvLut[R][G][B].V;
        }

        dstBuffer[0] = color;
        dstBuffer[1] = color;
        dstBuffer[2] = color;

        srcBuffer += 3;
        dstBuffer += 3;
    }
}
