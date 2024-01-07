#ifndef IMGFORMAT_H
#define IMGFORMAT_H

#include <string>

enum class ImgFormat {
    I420,
    I422,
    I444,
    UYVY,
    RGB,
    UNDEF
};

namespace yuvp {
static inline std::string toString(ImgFormat format) {
    if (format == ImgFormat::I420) {
        return "I420";
    } else if (format == ImgFormat::I422) {
        return "I422";
    } else if (format == ImgFormat::I444) {
        return "I444";
    } else if (format == ImgFormat::UYVY) {
        return "UYVY";
    } else if (format == ImgFormat::RGB) {
        return "RGB";
    } else if (format == ImgFormat::UNDEF) {
        return "UNDEF";
    }
    throw std::runtime_error("Unsupported image format " + std::to_string(static_cast<int>(format)));
}
}

#endif // IMGFORMAT_H
