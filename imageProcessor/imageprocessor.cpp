#include "imageprocessor.h"

ImageProcessor::ImageProcessor() {}

std::unique_ptr<RgbImage>
ImageProcessor::processImage(std::unique_ptr<RgbImage> src) const {
    if (src && m_nextImageProcessor) {
        auto dst = m_nextImageProcessor->processImage(std::move(src));
        return dst;
    }
    return src;
}

void
ImageProcessor::processImage(RgbImage& src) const {
    if (m_nextImageProcessor) {
        m_nextImageProcessor->processImage(src);
    }
}

std::unique_ptr<I444Image>
ImageProcessor::processImage(std::unique_ptr<I444Image> src) const {
    if (src && m_nextImageProcessor) {
        auto dst = m_nextImageProcessor->processImage(std::move(src));
        return dst;
    }
    return src;}

void
ImageProcessor::processImage(I444Image& src) const {
    if (m_nextImageProcessor) {
        m_nextImageProcessor->processImage(src);
    }
}

void
ImageProcessor::addImageProcessor(std::unique_ptr<ImageProcessor> nextImageProcessor) {
    if (m_nextImageProcessor) {
        m_nextImageProcessor->addImageProcessor(std::move(nextImageProcessor));
    } else {
        m_nextImageProcessor = std::move(nextImageProcessor);
    }
}

