QT       += core gui
QT       += network
QT       += multimedia multimediawidgets
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# GStreamer를 pkg-config로 설정
PKGCONFIG += gstreamer-1.0 gstreamer-video-1.0 gstreamer-rtsp-server-1.0

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    ChatProgram-Server_ko_KR.ts
CONFIG += lrelease
CONFIG += embed_translations

# Include paths for GStreamer and GLib
INCLUDEPATH += /opt/homebrew/Cellar/glib/2.82.1/include/glib-2.0 \
               /opt/homebrew/Cellar/glib/2.82.1/lib/glib-2.0/include \
               /opt/homebrew/Cellar/gstreamer/1.24.8/include/gstreamer-1.0 \
               /opt/homebrew/Cellar/gstreamer/1.24.8/include/gst

LIBS += -L/opt/homebrew/Cellar/glib/2.82.1/lib \
        -L/opt/homebrew/Cellar/gstreamer/1.24.8/lib \
        -lgstreamer-1.0 \
        -lgstapp-1.0 \
        -lgstbase-1.0 \
        -lgstvideo-1.0 \
        -lgstpbutils-1.0 \
        -lgstplay-1.0 \
        -lgstnet-1.0 \
        -lgstrtspserver-1.0 \
        -lglib-2.0 \
        -lgobject-2.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
