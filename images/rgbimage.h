#ifndef RGBIMAGE_H
#define RGBIMAGE_H

#include <vector>
#include "imagebase.h"

class RgbImage : public ImageBase
{
public:
    RgbImage(int width, int height, bool isInterlaced);
    ~RgbImage() override;

    RgbImage(const RgbImage&);
    RgbImage& operator=(const RgbImage&);

    uint8_t *getBuffer() override;
    const uint8_t *getBuffer() const override;

    void resize(int width, int height) override;

    std::unique_ptr<RgbImage> convertToRgb() const override;
    void convertToRgb(const std::unique_ptr<RgbImage> &dst) const override;
    std::unique_ptr<I444Image> convertToI444() const override;
    void convertToI444(const std::unique_ptr<I444Image>&) const override;

    bool readImage(FileDescriptor &file, int index = -1) override;

private:
    std::vector<uint8_t> m_pixels;
};

#endif // RGBIMAGE_H
