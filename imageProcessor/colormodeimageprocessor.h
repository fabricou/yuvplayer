#ifndef COLORMODEIMAGEPROCESSOR_H
#define COLORMODEIMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "../utils/colorMode.h"

class ColorModeImageProcessor : public ImageProcessor
{
public:
    ColorModeImageProcessor(ColorMode colorMode);
    void processImage(RgbImage& src) const override;
    std::unique_ptr<RgbImage> processImage(std::unique_ptr<RgbImage> src) const override;
private:
    ColorMode m_colorMode{ColorMode::UNDEF};
};

#endif // COLORMODEIMAGEPROCESSOR_H
