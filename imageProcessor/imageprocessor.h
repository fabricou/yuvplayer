#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <memory>
#include "../images/rgbimage.h"

class ImageProcessor
{
public:
    ImageProcessor();
    virtual ~ImageProcessor() = default;

    virtual void processImage(RgbImage& src) const;
    virtual std::unique_ptr<RgbImage> processImage(std::unique_ptr<RgbImage> src) const;
    void addImageProcessor(std::unique_ptr<ImageProcessor> nextImageProcessor);

protected:
    std::unique_ptr<ImageProcessor> m_nextImageProcessor;
};

#endif // IMAGEPROCESSOR_H
