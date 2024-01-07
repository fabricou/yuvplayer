#include "interlacedimageprocessor.h"
#include <cstring>

InterlacedImageProcessor::InterlacedImageProcessor()
    : ImageProcessor()
{
}

std::unique_ptr<RgbImage>
InterlacedImageProcessor::processImage(std::unique_ptr<RgbImage> src) const {
    std::unique_ptr<RgbImage> dst = std::make_unique<RgbImage>(src->getWidth(), src->getHeight(), src->isInterlaced());
    src->clone(*dst);
    processImage(*dst);
    return dst;
}

void
InterlacedImageProcessor::processImage(RgbImage& src) const {
    ImageProcessor::processImage(src);
    auto clone = src.clone();
    uint8_t *srcBuffer = clone->getBuffer();
    //top field
    uint8_t *dstBuffer = src.getBuffer();
    for (int i = 0; i < clone->getHeight()/2; ++i) {
        std::memcpy(dstBuffer, srcBuffer, clone->getWidth()*3);
        dstBuffer += clone->getWidth()*3;
        srcBuffer += (clone->getWidth()*3*2);
    }
    //bottom field
    srcBuffer = clone->getBuffer();
    srcBuffer += clone->getWidth()*3;
    for (int i = 0; i < clone->getHeight()/2; ++i) {
        std::memcpy(dstBuffer, srcBuffer, clone->getWidth()*3);
        dstBuffer += clone->getWidth()*3;
        srcBuffer += (clone->getWidth()*3*2);
    }
 }

std::unique_ptr<I444Image>
InterlacedImageProcessor::processImage(std::unique_ptr<I444Image> src) const {
    std::unique_ptr<I444Image> dst = std::make_unique<I444Image>(src->getWidth(), src->getHeight(), src->isInterlaced());
    src->clone(*dst);
    processImage(*dst);
    return dst;
}

void
InterlacedImageProcessor::processImage(I444Image& src) const {
    ImageProcessor::processImage(src);
    auto clone = src.clone();
    int offset = src.getWidth()*src.getHeight();
    uint8_t *srcBufferY = clone->getBuffer();
    uint8_t *srcBufferU = srcBufferY + offset;
    uint8_t *srcBufferV = srcBufferU + offset;
    //top field
    uint8_t *dstBufferY = src.getBuffer();
    uint8_t *dstBufferU = dstBufferY + offset;
    uint8_t *dstBufferV = dstBufferU + offset;
    for (int i = 0; i < clone->getHeight()/2; ++i) {
        std::memcpy(dstBufferY, srcBufferY, clone->getWidth());
        std::memcpy(dstBufferU, srcBufferU, clone->getWidth());
        std::memcpy(dstBufferV, srcBufferV, clone->getWidth());
        dstBufferY += clone->getWidth();
        dstBufferU += clone->getWidth();
        dstBufferV += clone->getWidth();
        srcBufferY += (clone->getWidth()*2);
        srcBufferU += (clone->getWidth()*2);
        srcBufferV += (clone->getWidth()*2);
    }
    //bottom field
    srcBufferY = clone->getBuffer() + clone->getWidth();
    srcBufferU = srcBufferY + offset;
    srcBufferV = srcBufferU + offset;
    for (int i = 0; i < clone->getHeight()/2; ++i) {
        std::memcpy(dstBufferY, srcBufferY, clone->getWidth());
        std::memcpy(dstBufferU, srcBufferU, clone->getWidth());
        std::memcpy(dstBufferV, srcBufferV, clone->getWidth());
        dstBufferY += clone->getWidth();
        dstBufferU += clone->getWidth();
        dstBufferV += clone->getWidth();
        srcBufferY += (clone->getWidth()*2);
        srcBufferU += (clone->getWidth()*2);
        srcBufferV += (clone->getWidth()*2);
    }
}
