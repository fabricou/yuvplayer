#ifndef I444IMAGE_H
#define I444IMAGE_H

#include <vector>
#include "imagebase.h"

class I444Image  : public ImageBase
{
public:
    I444Image(int width, int height, bool isInterlaced);
    ~I444Image() override = default;

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


#endif // I444IMAGE_H
