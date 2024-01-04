#ifndef GRIDIMAGEPROCESSOR_H
#define GRIDIMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "../utils/grid.h"

class GridImageProcessor : public ImageProcessor
{
public:
    GridImageProcessor(Grid grid);
    void processImage(RgbImage& src) const override;
    std::unique_ptr<RgbImage> processImage(std::unique_ptr<RgbImage> src) const override;
private:
    Grid m_grid{Grid::UNDEF};

};

#endif // GRIDIMAGEPROCESSOR_H
