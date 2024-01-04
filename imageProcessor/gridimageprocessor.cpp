#include "gridimageprocessor.h"

GridImageProcessor::GridImageProcessor(Grid grid)
    : ImageProcessor(),
    m_grid(grid)
{
}

std::unique_ptr<RgbImage>
GridImageProcessor::processImage(std::unique_ptr<RgbImage> src) const {
    std::unique_ptr<RgbImage> dst = std::make_unique<RgbImage>(src->getWidth(), src->getHeight());
    src->clone(*dst);
    processImage(*dst);
    return dst;
}

void
GridImageProcessor::processImage(RgbImage& src) const {
    ImageProcessor::processImage(src);
    if (m_grid == Grid::UNDEF || m_grid == Grid::NONE) {
        return;
    }
    uint8_t gridColor = (m_grid == Grid::BLACK) ? 0 : 255;
    uint8_t *dstBuffer = src.getBuffer();
    for (int i = 0; i < src.getHeight(); ++i) {
        for (int j = 0; j < src.getWidth(); ++j) {
            if (i%16 == 0 || j%16 == 0) {
                dstBuffer[(j*3) + 0] = gridColor;
                dstBuffer[(j*3) + 1] = gridColor;
                dstBuffer[(j*3) + 2] = gridColor;
            }
        }
        dstBuffer += 3*src.getWidth();
    }
}
