#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>

#include "images/rgbimage.h"

namespace Ui {
class ImageWidget;
}

class ImageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ImageWidget(QWidget *parent = 0);
    ~ImageWidget();
    void displayImage(const RgbImage &rgbImage, int currentImageIndex, int maxImageIndex);

protected:
    void paintEvent(QPaintEvent* event);

private:
    Ui::ImageWidget *ui;
    std::unique_ptr<QImage> m_image{nullptr};
};

#endif // IMAGEWIDGET_H
