#include "imagewidget.h"
#include "ui_imagewidget.h"


#include <QPainter>
#include <QDebug>

void
ImageWidget::displayImage(const RgbImage &rgbImage, int currentImageIndex, int maxImageIndex) {
    if (!m_image || m_image->width() != rgbImage.getWidth() || m_image->height() != rgbImage.getHeight()) {
        m_image.reset(new QImage(rgbImage.getWidth(),
                                 rgbImage.getHeight(),
                                 QImage::Format_RGB888));
    }
    uchar *data = m_image->bits();
    memcpy(data, rgbImage.getBuffer(), rgbImage.getWidth()*rgbImage.getHeight()*3);
    QString title = "im " + QString::number(currentImageIndex) + "/" + QString::number(maxImageIndex);
    setWindowTitle(title);
    repaint();
}

ImageWidget::ImageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ImageWidget)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Dialog);
}

ImageWidget::~ImageWidget()
{
    delete ui;
}

void
ImageWidget::paintEvent(QPaintEvent*) {
    if (!m_image) { return; }
    this->setFixedSize(m_image->width(), m_image->height());
    QPainter painter(this);
    painter.drawImage(rect(), *m_image, m_image->rect());
    painter.end();
}
