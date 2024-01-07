#ifndef ZOOMIMAGEPROCESSOR_H
#define ZOOMIMAGEPROCESSOR_H

#include "imageprocessor.h"
#include "../utils/zoom.h"

class ZoomImageProcessor : public ImageProcessor
{
public:
    ZoomImageProcessor(Zoom zoom);
    void processImage(RgbImage& src) const override;
    std::unique_ptr<RgbImage> processImage(std::unique_ptr<RgbImage> src) const override;
    void processImage(I444Image& src) const override;
    std::unique_ptr<I444Image> processImage(std::unique_ptr<I444Image> src) const override;
private:
    Zoom m_zoom{Zoom::UNDEF};
};

#endif // ZOOMIMAGEPROCESSOR_H
