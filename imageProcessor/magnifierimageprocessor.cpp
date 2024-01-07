#include "magnifierimageprocessor.h"

MagnifierImageProcessor::MagnifierImageProcessor(Magnifier::Point point)
    : ImageProcessor(),
    m_point(point)
{}

std::unique_ptr<RgbImage>
MagnifierImageProcessor::processImage(std::unique_ptr<RgbImage> src) const {
    std::unique_ptr<RgbImage> dst = std::make_unique<RgbImage>(src->getWidth(), src->getHeight(), src->isInterlaced());
    src->clone(*dst);
    processImage(*dst);
    return dst;
}

void
MagnifierImageProcessor::processImage(RgbImage& src) const {
    ImageProcessor::processImage(src);

    int width = src.getWidth();
    int height = src.getHeight();
    int x = m_point.x;
    int y = m_point.y;
    int x0 = (x >= width/2)  ? x - (width/2) : 0;
    int y0 = (y >= height/2) ? y - (height/2) : 0;

    auto clone = src.clone();
    uint8_t *srcBuffer = clone->getBuffer();
    uint8_t *dstBuffer = src.getBuffer();

    srcBuffer += ((x0*3) + y0*width*3);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int k = j*3;
            int l = (j/2)*3;
            dstBuffer[k] = srcBuffer[l];
            dstBuffer[k+1] = srcBuffer[l+1];
            dstBuffer[k+2] = srcBuffer[l+2];
        }
        dstBuffer += width*3;
        srcBuffer += width*3*(i%2 == 1);
    }
}

std::unique_ptr<I444Image>
MagnifierImageProcessor::processImage(std::unique_ptr<I444Image> src) const {
    std::unique_ptr<I444Image> dst = std::make_unique<I444Image>(src->getWidth(), src->getHeight(), src->isInterlaced());
    src->clone(*dst);
    processImage(*dst);
    return dst;
}

void
MagnifierImageProcessor::processImage(I444Image& src) const {
    ImageProcessor::processImage(src);

    int width = src.getWidth();
    int height = src.getHeight();
    int x = m_point.x;
    int y = m_point.y;
    int x0 = (x >= width/2)  ? x - (width/2) : 0;
    int y0 = (y >= height/2) ? y - (height/2) : 0;

    auto clone = src.clone();
    uint8_t *srcBuffer = clone->getBuffer();
    uint8_t *dstBuffer = src.getBuffer();
    int offset = src.getWidth()*src.getHeight();

    uint8_t *srcBufferY = &srcBuffer[x0 + y0*width];
    uint8_t *srcBufferU = srcBufferY + offset;
    uint8_t *srcBufferV = srcBufferU + offset;

    uint8_t *dstBufferY = dstBuffer;
    uint8_t *dstBufferU = dstBufferY + offset;
    uint8_t *dstBufferV = dstBufferU + offset;


    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            dstBufferY[j] = srcBufferY[j/2];
            dstBufferU[j] = srcBufferU[j/2];
            dstBufferV[j] = srcBufferV[j/2];
        }
        srcBufferY += width*(i%2 == 1);
        srcBufferU += width*(i%2 == 1);
        srcBufferV += width*(i%2 == 1);
        dstBufferY += width;
        dstBufferU += width;
        dstBufferV += width;
    }
}
