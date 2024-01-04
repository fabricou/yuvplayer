#include "magnifierimageprocessor.h"

MagnifierImageProcessor::MagnifierImageProcessor(Magnifier::Point point)
    : ImageProcessor(),
    m_point(point)
{}

std::unique_ptr<RgbImage>
MagnifierImageProcessor::processImage(std::unique_ptr<RgbImage> src) const {
    std::unique_ptr<RgbImage> dst = std::make_unique<RgbImage>(src->getWidth(), src->getHeight());
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
        srcBuffer += width*3*(i%2 == 0);
    }
}
