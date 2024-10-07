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
# 환경에 따라 다른 경로를 설정하는 부분 (macOS, Raspberry Pi, 기타)
# macOS 설정
macx {
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
            -lgstnet-1.0 \
            -lgstrtspserver-1.0 \
            -lglib-2.0 \
            -lgobject-2.0
}

# Raspberry Pi 설정
linux:!macx {
    INCLUDEPATH += /usr/include/gstreamer-1.0 \
                   /usr/include/glib-2.0 \
                   /usr/lib/aarch64-linux-gnu/gstreamer-1.0 \
                   /usr/lib/aarch64-linux-gnu/glib-2.0/include

    LIBS += -L/usr/lib/aarch64-linux-gnu \
            -lgstreamer-1.0 \
            -lgstapp-1.0 \
            -lgstbase-1.0 \
            -lgstvideo-1.0 \
            -lgstpbutils-1.0 \
            -lgstnet-1.0 \
            -lgstrtspserver-1.0 \
            -lglib-2.0 \
            -lgobject-2.0
}

# 기타 Unix 계열 시스템 설정
unix:!macx:!linux {
    INCLUDEPATH += /usr/include/gstreamer-1.0 \
                   /usr/include/glib-2.0

    LIBS += -L/usr/lib \
            -lgstreamer-1.0 \
            -lgstapp-1.0 \
            -lgstbase-1.0 \
            -lgstvideo-1.0 \
            -lgstpbutils-1.0 \
            -lgstnet-1.0 \
            -lgstrtspserver-1.0 \
            -lglib-2.0 \
            -lgobject-2.0
}
