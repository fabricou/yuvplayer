#ifndef IMAGEPROCESSOR_H
#define IMAGEPROCESSOR_H

#include <memory>
#include "../images/rgbimage.h"
#include "../images/i444image.h"

class ImageProcessor
{
public:
    ImageProcessor();
    virtual ~ImageProcessor() = default;

    ImageProcessor(const ImageProcessor &) = delete;
    ImageProcessor& operator=(const ImageProcessor &) = delete;

    virtual void processImage(RgbImage& src) const;
    virtual std::unique_ptr<RgbImage> processImage(std::unique_ptr<RgbImage> src) const;
    virtual void processImage(I444Image& src) const;
    virtual std::unique_ptr<I444Image> processImage(std::unique_ptr<I444Image> src) const;

    void addImageProcessor(std::unique_ptr<ImageProcessor> nextImageProcessor);

protected:
    std::unique_ptr<ImageProcessor> m_nextImageProcessor;
};

#endif // IMAGEPROCESSOR_H
