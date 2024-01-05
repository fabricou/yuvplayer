#ifndef IIMAGE_H
#define IIMAGE_H

#include <memory>
#include "imgformat.h"
#include "fileDescriptor/filedescriptor.h"

class RgbImage;

class IImage
{
public:
    IImage(int width, int height, ImgFormat format, bool isInterlaced);
    virtual ~IImage();

    //IImage(const IImage&) = delete;
    //IImage& operator=(const IImage&) = delete;

    virtual uint8_t *getBuffer() = 0;
    virtual const uint8_t *getBuffer() const = 0;

    virtual void resize(int width, int height) = 0;

    virtual std::unique_ptr<IImage> clone() const = 0;
    virtual void clone(IImage& dst) const = 0;

    virtual std::unique_ptr<RgbImage> convertToRgb() const = 0;
    virtual void convertToRgb(const std::unique_ptr<RgbImage>&) const = 0;

    virtual bool readImage(FileDescriptor &file, int index = -1) = 0;
    virtual size_t getSizeInBytes() const = 0;

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    ImgFormat getFormat() const { return m_format; }
    bool isInterlaced() const { return m_isInterlaced; }

    static std::unique_ptr<IImage> create(int width, int height, ImgFormat format, bool isInterlaced);
protected:
    static void RGBfromYUV(int& R, int& G, int& B, int Y, int U, int V);

    int m_width{0};
    int m_height{0};
    ImgFormat m_format{ImgFormat::UNDEF};
    bool m_isInterlaced{false};
};

#endif // IIMAGE_H
