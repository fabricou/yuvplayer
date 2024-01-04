#ifndef MAGNIFIERIMAGEPROCESSOR_H
#define MAGNIFIERIMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "../utils/magnifier.h"

class MagnifierImageProcessor : public ImageProcessor
{
public:
    MagnifierImageProcessor(Magnifier::Point point);
    void processImage(RgbImage& src) const override;
    std::unique_ptr<RgbImage> processImage(std::unique_ptr<RgbImage> src) const override;
private:
    Magnifier::Point m_point;
};

#endif // MAGNIFIERIMAGEPROCESSOR_H
