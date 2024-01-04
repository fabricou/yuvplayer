#ifndef USERINTERFACEMAINWINDOW_H
#define USERINTERFACEMAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "imagewidget.h"
#include "controller/playercontroller.h"
#include "utils/magnifier.h"

class MouseClickEater : public QObject
{
    Q_OBJECT
public:
    MouseClickEater(QObject *parent = nullptr) : QObject(parent) {}
protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};

namespace Ui {
class UserInterfaceMainWindow;
}

class UserInterfaceMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit UserInterfaceMainWindow(QWidget *parent = 0);
    ~UserInterfaceMainWindow();

signals:
    void signalSendCreateScreen();
    void signalSendDeleteScreen();
    void signalSendDisplayImage(const RgbImage& image);
    void signalSendIsScreenAvailable(bool &);

private slots:
    void signalReceiveCreateScreen();
    void signalReceiveDeleteScreen();
    void signalReceiveDisplayImage(RgbImage image);
    void signalReceiveIsScreenAvailable(bool &);

private:
    friend class MouseClickEater;

    void on_pushButtonOpen_clicked();
    void on_pushButtonClose_clicked();
    void on_lineEditWidth_returnPressed();
    void on_lineEditHeight_returnPressed();
    void on_comboBoxYuvFormat_currentIndexChanged(int /*index*/);
    void on_radioButtonAutoSize_clicked(bool checked);
    void on_radioButtonManualSize_clicked(bool checked);
    void on_toolButtonFastBackward_clicked();
    void on_toolButtonPreviousFrame_clicked();
    void on_toolButtonNextFrame_clicked();
    void on_toolButtonFastForward_clicked();
    void on_toolButtonPlayBackward_clicked();
    void on_toolButtonPause_clicked();
    void on_toolButtonPlayForward_clicked();
    void on_lineEditGoTo_returnPressed();
    void on_radioButtonProgressive_clicked(bool checked);
    void on_radioButtonInterlaced_clicked(bool checked);
    void on_pushButtonZoom_clicked();
    void on_pushButtonYUV_clicked();
    void on_toolButtonGrid_clicked();
    void on_toolButtonMagnifier_clicked();

    void createController();
    void sendConfigToController(int imageIndex = -1);

    Ui::UserInterfaceMainWindow *ui;
    std::unique_ptr<ImageWidget> m_imageWidget{nullptr};
    std::unique_ptr<PlayerController> m_playerController;
    Magnifier m_magnifierFactor;

    std::unique_ptr<MouseClickEater> m_mouseClickEater; //catch mouse click on image display widget
};

#endif // USERINTERFACEMAINWINDOW_H
