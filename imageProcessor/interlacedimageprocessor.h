#ifndef INTERLACEDIMAGEPROCESSOR_H
#define INTERLACEDIMAGEPROCESSOR_H

#include "imageprocessor.h"

class InterlacedImageProcessor : public ImageProcessor
{
public:
    InterlacedImageProcessor();

    std::unique_ptr<RgbImage> processImage(std::unique_ptr<RgbImage> src) const override;
    void processImage(RgbImage& src) const override;

};

#endif // INTERLACEDIMAGEPROCESSOR_H
