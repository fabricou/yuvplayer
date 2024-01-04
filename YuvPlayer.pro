#-------------------------------------------------
#
# Project created by QtCreator 2023-12-24T15:56:44
#
#-------------------------------------------------

QT       += core gui
CONFIG   += c++20
#CONFIG   += sanitizer
#CONFIG   += sanitize_address

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = YuvPlayer
TEMPLATE = app


SOURCES += main.cpp\
    controller/playercontroller.cpp \
    fileDescriptor/filedescriptor.cpp \
    imageProcessor/colormodeimageprocessor.cpp \
    imageProcessor/gridimageprocessor.cpp \
    imageProcessor/imageprocessor.cpp \
    imageProcessor/interlacedimageprocessor.cpp \
    imageProcessor/magnifierimageprocessor.cpp \
    imageProcessor/zoomimageprocessor.cpp \
        userinterfacemainwindow.cpp \
    images/iimage.cpp \
    images/rgbimage.cpp \
    images/i420image.cpp \
    imagewidget.cpp \
    utils/magnifier.cpp

HEADERS  += userinterfacemainwindow.h \
    controller/playercontroller.h \
    fileDescriptor/filedescriptor.h \
    imageProcessor/colormodeimageprocessor.h \
    imageProcessor/gridimageprocessor.h \
    imageProcessor/imageprocessor.h \
    imageProcessor/interlacedimageprocessor.h \
    imageProcessor/magnifierimageprocessor.h \
    imageProcessor/zoomimageprocessor.h \
    images/iimage.h \
    images/rgbimage.h \
    images/imgformat.h \
    images/i420image.h \
    imagewidget.h \
    utils/colorMode.h \
    utils/grid.h \
    utils/magnifier.h \
    utils/zoom.h

FORMS    += userinterfacemainwindow.ui \
    imagewidget.ui

RESOURCES += \
    icons/icons.qrc

DISTFILES += \
    README.md
