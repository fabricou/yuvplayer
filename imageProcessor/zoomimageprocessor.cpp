#include "zoomimageprocessor.h"

static void zoomx0_5(RgbImage &src) {
    RgbImage clone(src.getWidth(), src.getHeight());
    src.clone(clone);
    auto width  = src.getWidth() / 2;
    auto height = src.getHeight() / 2;
    src.resize(width, height);
    uint8_t *srcBuffer = clone.getBuffer();
    uint8_t *dstBuffer = src.getBuffer();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int k = 3*j; //because rgb value for each point
            dstBuffer[k] = srcBuffer[k*2];
            dstBuffer[k+1] = srcBuffer[k*2+1];
            dstBuffer[k+2] = srcBuffer[k*2+2];
        }
        dstBuffer += 3*src.getWidth();
        srcBuffer += 3*clone.getWidth()*2;
    }
}

static void zoomx2(RgbImage &src) {
    RgbImage clone(src.getWidth(), src.getHeight());
    src.clone(clone);
    auto width  = src.getWidth() * 2;
    auto height = src.getHeight() * 2;
    src.resize(width, height);
    uint8_t *srcBuffer = clone.getBuffer();
    uint8_t *dstBuffer = src.getBuffer();
    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int k0 = 3*j; //because rgb value for each point
            int k1 = 3*(j/2);
            dstBuffer[k0] = srcBuffer[k1];
            dstBuffer[k0+1] = srcBuffer[k1+1];
            dstBuffer[k0+2] = srcBuffer[k1+2];
        }
        dstBuffer += 3*src.getWidth();
        srcBuffer += 3*clone.getWidth()*(i%2==1);
    }
}

ZoomImageProcessor::ZoomImageProcessor(Zoom zoom)
    : ImageProcessor(),
    m_zoom {zoom} {
}

std::unique_ptr<RgbImage>
ZoomImageProcessor::processImage(std::unique_ptr<RgbImage> src) const {
    std::unique_ptr<RgbImage> dst = std::make_unique<RgbImage>(src->getWidth(), src->getHeight());
    src->clone(*dst);
    processImage(*dst);
    return dst;
}

void
ZoomImageProcessor::processImage(RgbImage& src) const {
    ImageProcessor::processImage(src);
    if (m_zoom == Zoom::x0_5) {
        zoomx0_5(src);
    } else if (m_zoom == Zoom::x2) {
        zoomx2(src);
    }
}

