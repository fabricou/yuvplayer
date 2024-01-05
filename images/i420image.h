#ifndef I420IMAGE_H
#define I420IMAGE_H

#include <vector>
#include "iimage.h"

class I420Image  : public IImage
{
public:
    I420Image(int width, int height, bool isInterlaced);
    ~I420Image() override = default;

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

#endif // I420IMAGE_H
