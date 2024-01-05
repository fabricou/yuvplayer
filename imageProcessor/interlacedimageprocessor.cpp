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
