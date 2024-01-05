#ifndef RGBIMAGE_H
#define RGBIMAGE_H

#include <vector>
#include "iimage.h"

class RgbImage : public IImage
{
public:
    RgbImage(int width, int height, bool isInterlaced);
    ~RgbImage() override;

    RgbImage(const RgbImage&);
    RgbImage& operator=(const RgbImage&);

    uint8_t *getBuffer() override;
    const uint8_t *getBuffer() const override;

    void resize(int width, int height) override;

    std::unique_ptr<IImage> clone() const override;
    void clone(IImage& dst) const override;

    std::unique_ptr<RgbImage> convertToRgb() const override;
    void convertToRgb(const std::unique_ptr<RgbImage> &dst) const override;

    bool readImage(FileDescriptor &file, int index = -1) override;
    size_t getSizeInBytes() const override;

private:
    std::vector<uint8_t> m_pixels;
    size_t m_sizeInBytes{0};
};

#endif // RGBIMAGE_H
