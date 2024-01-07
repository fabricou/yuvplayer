#ifndef IMAGEBASE_H
#define IMAGEBASE_H

#include <memory>
#include "imgformat.h"
#include "fileDescriptor/filedescriptor.h"

class RgbImage;
class I444Image;

class ImageBase
{
public:
    ImageBase(int width, int height, ImgFormat format, bool isInterlaced);
    virtual ~ImageBase();
    ImageBase(const ImageBase&);
    ImageBase& operator=(const ImageBase&);

    std::unique_ptr<ImageBase> clone() const;
    void clone(ImageBase& dst) const;

    virtual uint8_t *getBuffer() = 0;
    virtual const uint8_t *getBuffer() const = 0;

    virtual void resize(int width, int height) = 0;

    virtual std::unique_ptr<RgbImage> convertToRgb() const = 0;
    virtual void convertToRgb(const std::unique_ptr<RgbImage>&) const = 0;
    virtual std::unique_ptr<I444Image> convertToI444() const = 0;
    virtual void convertToI444(const std::unique_ptr<I444Image>&) const = 0;

    virtual bool readImage(FileDescriptor &file, int index = -1) = 0;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    ImgFormat getFormat() const { return m_format; }
    bool isInterlaced() const { return m_isInterlaced; }
    size_t getSizeInBytes() const { return m_sizeInBytes; }

    static std::unique_ptr<ImageBase> create(int width, int height, ImgFormat format, bool isInterlaced);
protected:
    static void RGBfromYUV(int& R, int& G, int& B, int Y, int U, int V);

    int m_width{0};
    int m_height{0};
    bool m_isInterlaced{false};
    ImgFormat m_format{ImgFormat::UNDEF};
    size_t m_sizeInBytes{0};
};

#endif // IMAGEBASE_H
