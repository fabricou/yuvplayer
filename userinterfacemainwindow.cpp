#include "userinterfacemainwindow.h"
#include "ui_userinterfacemainwindow.h"

#include <QFileDialog>
#include <string>

#include "images/rgbimage.h"
#include "fileDescriptor/filedescriptor.h"
#include "images/i420image.h"
#include "utils/zoom.h"
#include "utils/colorMode.h"
#include "utils/grid.h"

bool MouseClickEater::eventFilter(QObject *obj, QEvent *event) {

    auto parent = static_cast<UserInterfaceMainWindow *>(obj->parent());
    bool isMagnifierEnabled = parent->ui->toolButtonMagnifier->isChecked();

    if (isMagnifierEnabled && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            int x = mouseEvent->position().x();
            int y = mouseEvent->position().y();
            parent->m_magnifierFactor.increase(Magnifier::Point(x, y));
        } else if (mouseEvent->button() == Qt::RightButton) {
            parent->m_magnifierFactor.decrease();
        }
        parent->sendConfigToController();
        //qDebug() << "Ate mouse button press x " << x << " y " << y << " magnifier " << (int) parent->m_magnifierFactor ;
        return true;
    } else {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

static ImgFormat convertComboStringToImgFormat(QString format) {
    if (format == "I420") {
        return ImgFormat::I420;
    } else if (format == "UYVY") {
        return ImgFormat::UYVY;
    }
    return ImgFormat::UNDEF;
}

static ColorMode convertPushButtonYuvToColor(QString yuv) {
    if (yuv == "YUV") {
        return ColorMode::YUV;
    } else if (yuv == "Y") {
        return ColorMode::Y;
    } else if (yuv == "U") {
        return ColorMode::U;
    } else if (yuv == "V") {
        return ColorMode::V;
    }
    return ColorMode::UNDEF;
}

static Grid convertToolButtonGridToGrid(QString text) {
    if (text == "NoGrid") {
        return Grid::NONE;
    } else if (text == "WhiteGrid") {
        return Grid::WHITE;
    } else if (text == "BlackGrid") {
        return Grid::BLACK;
    }
    return Grid::UNDEF;
}

static Zoom convertPushButtonZoomToZoom(QString zoom) {
    if (zoom == "Zoom x0.5") {
        return Zoom::x0_5;
    } else if (zoom == "Zoom x1") {
        return Zoom::x1;
    } else if (zoom == "Zoom x2") {
        return Zoom::x2;
    }
    return Zoom::UNDEF;
}

UserInterfaceMainWindow::UserInterfaceMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::UserInterfaceMainWindow)
{
    ui->setupUi(this);
    setWindowTitle("YUV Player");
    createController();
    m_mouseClickEater = std::make_unique<MouseClickEater>(this);

    connect(ui->pushButtonOpen, &QPushButton::clicked, this, &UserInterfaceMainWindow::on_pushButtonOpen_clicked);
    connect(ui->pushButtonClose, &QPushButton::clicked, this, &UserInterfaceMainWindow::on_pushButtonClose_clicked);
    connect(ui->lineEditWidth, &QLineEdit::returnPressed, this, &UserInterfaceMainWindow::on_lineEditWidth_returnPressed);
    connect(ui->lineEditHeight, &QLineEdit::returnPressed, this, &UserInterfaceMainWindow::on_lineEditHeight_returnPressed);
    connect(ui->comboBoxYuvFormat, &QComboBox::currentIndexChanged, this, &UserInterfaceMainWindow::on_comboBoxYuvFormat_currentIndexChanged);
    connect(ui->radioButtonAutoSize, &QRadioButton::clicked, this, &UserInterfaceMainWindow::on_radioButtonAutoSize_clicked);
    connect(ui->radioButtonManualSize, &QRadioButton::clicked, this, &UserInterfaceMainWindow::on_radioButtonManualSize_clicked);
    connect(ui->toolButtonFastBackward, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonFastBackward_clicked);
    connect(ui->toolButtonPreviousFrame, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonPreviousFrame_clicked);
    connect(ui->toolButtonNextFrame, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonNextFrame_clicked);
    connect(ui->toolButtonFastForward, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonFastForward_clicked);
    connect(ui->toolButtonPlayBackward, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonPlayBackward_clicked);
    connect(ui->toolButtonPause, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonPause_clicked);
    connect(ui->toolButtonPlayForward, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonPlayForward_clicked);
    connect(ui->lineEditGoTo, &QLineEdit::returnPressed, this, &UserInterfaceMainWindow::on_lineEditGoTo_returnPressed);
    connect(ui->radioButtonProgressive, &QRadioButton::clicked, this, &UserInterfaceMainWindow::on_radioButtonProgressive_clicked);
    connect(ui->radioButtonInterlaced, &QRadioButton::clicked, this, &UserInterfaceMainWindow::on_radioButtonInterlaced_clicked);
    connect(ui->pushButtonZoom, &QPushButton::clicked, this, &UserInterfaceMainWindow::on_pushButtonZoom_clicked);
    connect(ui->pushButtonYUV, &QPushButton::clicked, this, &UserInterfaceMainWindow::on_pushButtonYUV_clicked);
    connect(ui->toolButtonGrid, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonGrid_clicked);
    connect(ui->toolButtonMagnifier, &QToolButton::clicked, this, &UserInterfaceMainWindow::on_toolButtonMagnifier_clicked);

    //connect signal/slots for UI management from not-main threads
    connect(this, SIGNAL(signalSendCreateScreen()), this, SLOT(signalReceiveCreateScreen()), Qt::DirectConnection);
    connect(this, SIGNAL(signalSendDeleteScreen()), this, SLOT(signalReceiveDeleteScreen()), Qt::DirectConnection);
    connect(this, SIGNAL(signalSendDisplayImage(RgbImage)), this, SLOT(signalReceiveDisplayImage(RgbImage)), Qt::DirectConnection);
    connect(this, SIGNAL(signalSendIsScreenAvailable(bool&)), this, SLOT(signalReceiveIsScreenAvailable(bool&)), Qt::DirectConnection);

    ui->toolButtonFastBackward->setIcon(QIcon(":/fast-backward.png"));
    ui->toolButtonPreviousFrame->setIcon(QIcon(":/previous-frame.png"));
    ui->toolButtonNextFrame->setIcon(QIcon(":/next-frame.png"));
    ui->toolButtonFastForward->setIcon(QIcon(":/fast-forward.png"));
    ui->toolButtonPlayBackward->setIcon(QIcon(":/play-backward.png"));
    ui->toolButtonPause->setIcon(QIcon(":/pause.png"));
    ui->toolButtonPlayForward->setIcon(QIcon(":/play-forward.png"));
    ui->toolButtonGrid->setIcon(QIcon(":/grid.png"));
    ui->toolButtonMagnifier->setIcon(QIcon(":/magnifier.png"));

    QPalette palette = ui->toolButtonGrid->palette();
    palette.setBrush(QPalette::Base, QColor(qRgb(220,220,220)));
    palette.setBrush(QPalette::Button, QColor(qRgb(220,220,220)));
    ui->toolButtonFastBackward->setPalette(palette);
    ui->toolButtonPreviousFrame->setPalette(palette);
    ui->toolButtonNextFrame->setPalette(palette);
    ui->toolButtonFastForward->setPalette(palette);
    ui->toolButtonPlayBackward->setPalette(palette);
    ui->toolButtonPause->setPalette(palette);
    ui->toolButtonFastBackward->setPalette(palette);
    ui->toolButtonPlayForward->setPalette(palette);
    ui->toolButtonGrid->setPalette(palette);
    ui->toolButtonMagnifier->setPalette(palette);
    ui->pushButtonOpen->setPalette(palette);
    ui->pushButtonClose->setPalette(palette);
    ui->pushButtonYUV->setPalette(palette);
    ui->pushButtonZoom->setPalette(palette);

    ui->lineEditWidth->setValidator(new QIntValidator(0, 4000, this));
    ui->lineEditHeight->setValidator(new QIntValidator(0, 3000, this));
    ui->lineEditGoTo->setValidator(new QIntValidator(0, 1000000, this));

    ui->radioButtonAutoSize->setChecked(true);
    ui->radioButtonManualSize->setChecked(false);
    on_radioButtonAutoSize_clicked(true);
    ui->radioButtonProgressive->setChecked(true);
    ui->radioButtonInterlaced->setChecked(false);
    on_radioButtonProgressive_clicked(true);
    ui->toolButtonMagnifier->setCheckable(true);
}

void UserInterfaceMainWindow::signalReceiveCreateScreen(){
    if (!m_imageWidget) {
        m_imageWidget = std::make_unique<ImageWidget>(this);
        m_imageWidget->installEventFilter(m_mouseClickEater.get());
        m_imageWidget->setVisible(true);
    }
}

void UserInterfaceMainWindow::signalReceiveDeleteScreen(){
    m_imageWidget.reset(nullptr);
}

void UserInterfaceMainWindow::signalReceiveDisplayImage(RgbImage image){
    if(m_imageWidget) {
        m_imageWidget->displayImage(image);
    }
}

void UserInterfaceMainWindow::signalReceiveIsScreenAvailable(bool &isAvail){
    isAvail = (m_imageWidget && m_imageWidget->isVisible());
}

void
UserInterfaceMainWindow::createController() {
    auto createScreen = [this](){
        emit signalSendCreateScreen();
        /*
        if (!m_imageWidget) {
            m_imageWidget = std::make_unique<ImageWidget>(nullptr);
            m_imageWidget->setVisible(true);
        }
        */
    };

    auto deleteScreen = [this]() {
        emit signalSendDeleteScreen();
        //m_imageWidget.reset(nullptr);
    };

    auto isScreenAvailable = [this]() {
        bool isAvail = false;
        emit signalSendIsScreenAvailable(isAvail);
        return isAvail;
        //return (m_imageWidget && m_imageWidget->isVisible());
    };

    auto displayImage = [this](const RgbImage& image) {
        emit signalSendDisplayImage(image);
        /*
        if(m_imageWidget) {
            m_imageWidget->displayImage(image);
        }
        */
    };

    m_playerController = std::make_unique<PlayerController>(createScreen, deleteScreen, isScreenAvailable, displayImage);
}

void
UserInterfaceMainWindow::sendConfigToController(int imageIndex) {
    auto width = ui->lineEditWidth->text().toInt();
    auto height = ui->lineEditHeight->text().toInt();
    auto format = convertComboStringToImgFormat(ui->comboBoxYuvFormat->currentText());
    auto zoom = convertPushButtonZoomToZoom(ui->pushButtonZoom->text());
    auto color = convertPushButtonYuvToColor(ui->pushButtonYUV->text());
    auto grid = convertToolButtonGridToGrid(ui->toolButtonGrid->text());

    m_playerController->setHeight(height);
    m_playerController->setWidth(width);
    m_playerController->setImageFormat(format);
    m_playerController->setInterlacedMode(ui->radioButtonInterlaced->isChecked());
    m_playerController->setZoom(zoom);
    m_playerController->setColorMode(color);
    m_playerController->setGrid(grid);
    m_playerController->setMagnifier(m_magnifierFactor);
    if (imageIndex != -1) {
        m_playerController->setImageNumber(imageIndex);
    }
}

UserInterfaceMainWindow::~UserInterfaceMainWindow()
{
    delete ui;
}

void UserInterfaceMainWindow::on_pushButtonOpen_clicked()
{
    auto qtFileName = QFileDialog::getOpenFileName(this,
                                            tr("Open file"), "", tr("raw video files (*.yuv *.raw)"));
    std::string filename = qtFileName.toStdString();
    if (!filename.empty()) {
        m_playerController->openYuvFile(filename);
        if (ui->radioButtonAutoSize->isChecked()) {
            on_radioButtonAutoSize_clicked(true);
        } else {
            on_radioButtonManualSize_clicked(true);
        }
    }
}

void UserInterfaceMainWindow::on_pushButtonClose_clicked()
{
    m_imageWidget.reset(nullptr);
    createController();
}


void UserInterfaceMainWindow::on_lineEditWidth_returnPressed()
{
    sendConfigToController(0);
}


void UserInterfaceMainWindow::on_lineEditHeight_returnPressed()
{
    sendConfigToController(0);
}


void UserInterfaceMainWindow::on_comboBoxYuvFormat_currentIndexChanged(int /*index*/)
{
    sendConfigToController(0);
}


void UserInterfaceMainWindow::on_radioButtonAutoSize_clicked(bool checked)
{
    if (checked) {
        QPalette palette;
        palette.setColor(QPalette::Base, QColor(200,200,200));

        ui->lineEditWidth->setEnabled(false);
        ui->lineEditHeight->setEnabled(false);

        ui->lineEditWidth->setPalette(palette);
        ui->lineEditHeight->setPalette(palette);

        auto format = convertComboStringToImgFormat(ui->comboBoxYuvFormat->currentText());
        int width = 0, height = 0;
        m_playerController->computeAutoImageSize(format,width, height);
        if (width > 0 && height > 0) {
            ui->lineEditWidth->setText(QString::number(width));
            ui->lineEditHeight->setText(QString::number(height));
        }
        sendConfigToController(0);
    }
}

void UserInterfaceMainWindow::on_radioButtonManualSize_clicked(bool checked)
{
    if (checked) {
        QPalette palette;
        palette.setColor(QPalette::Base, QColor(255,255,255));

        ui->lineEditWidth->setEnabled(true);
        ui->lineEditHeight->setEnabled(true);

        ui->lineEditWidth->setPalette(palette);
        ui->lineEditHeight->setPalette(palette);
        sendConfigToController(0);
    }
}

void UserInterfaceMainWindow::on_toolButtonPlayBackward_clicked()
{
    m_playerController->playBackward();
}

void UserInterfaceMainWindow::on_toolButtonPause_clicked()
{
    m_playerController->pause();
}

void UserInterfaceMainWindow::on_toolButtonPlayForward_clicked()
{
    m_playerController->playForward();
}

void UserInterfaceMainWindow::on_toolButtonFastBackward_clicked()
{
    m_playerController->displayFirstFrame();
}

void UserInterfaceMainWindow::on_toolButtonPreviousFrame_clicked()
{
    m_playerController->displayPreviousFrame();
}

void UserInterfaceMainWindow::on_toolButtonNextFrame_clicked()
{
    m_playerController->displayNextFrame();
}

void UserInterfaceMainWindow::on_toolButtonFastForward_clicked()
{
    m_playerController->displayLastFrame();
}

void UserInterfaceMainWindow::on_lineEditGoTo_returnPressed()
{
    auto imageIndex = ui->lineEditGoTo->text().toInt();
    sendConfigToController(imageIndex);
}


void UserInterfaceMainWindow::on_radioButtonProgressive_clicked(bool checked)
{
    if (checked) {
        sendConfigToController();
    }
}

void UserInterfaceMainWindow::on_radioButtonInterlaced_clicked(bool checked)
{
    if (checked) {
        sendConfigToController();
    }
}

void UserInterfaceMainWindow::on_pushButtonZoom_clicked()
{
    if (ui->pushButtonZoom->text() == "Zoom x1") {
        ui->pushButtonZoom->setText("Zoom x2");
    } else if (ui->pushButtonZoom->text() == "Zoom x2") {
        ui->pushButtonZoom->setText("Zoom x0.5");
    } else if (ui->pushButtonZoom->text() == "Zoom x0.5") {
        ui->pushButtonZoom->setText("Zoom x1");
    } else {
        throw std::runtime_error("Invalid Zoom option");
    }
    sendConfigToController();
}

void UserInterfaceMainWindow::on_pushButtonYUV_clicked()
{
    if (ui->pushButtonYUV->text() == "YUV") {
        ui->pushButtonYUV->setText("Y");
    } else if (ui->pushButtonYUV->text() == "Y") {
        ui->pushButtonYUV->setText("U");
    } else if (ui->pushButtonYUV->text() == "U") {
        ui->pushButtonYUV->setText("V");
    } else if (ui->pushButtonYUV->text() == "V") {
        ui->pushButtonYUV->setText("YUV");
    } else {
        throw std::runtime_error("Invalid YUV color option");
    }
    sendConfigToController();
}

void UserInterfaceMainWindow::on_toolButtonGrid_clicked()
{
    if (ui->toolButtonGrid->text() == "NoGrid") {
        ui->toolButtonGrid->setText("BlackGrid");
    } else if (ui->toolButtonGrid->text() == "BlackGrid") {
        ui->toolButtonGrid->setText("WhiteGrid");
    } else if (ui->toolButtonGrid->text() == "WhiteGrid") {
        ui->toolButtonGrid->setText("NoGrid");
    }
    sendConfigToController();
}

void UserInterfaceMainWindow::on_toolButtonMagnifier_clicked()
{
    m_magnifierFactor.reset();
    sendConfigToController();
}

