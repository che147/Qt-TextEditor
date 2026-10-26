#-------------------------------------------------
#
# Project created by QtCreator
# 实验二  简单文本编辑器
#
#-------------------------------------------------

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG   += c++17

TARGET   = Notepad
TEMPLATE = app
VERSION  = 1.0.0

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    codeeditor.cpp \
    aboutdialog.cpp \
    finddialog.cpp \
    replacedialog.cpp

HEADERS += \
    mainwindow.h \
    codeeditor.h \
    aboutdialog.h \
    finddialog.h \
    replacedialog.h \
    themewin.h

FORMS += \
    mainwindow.ui \
    aboutdialog.ui \
    finddialog.ui \
    replacedialog.ui

RESOURCES += \
    resources.qrc

win32: LIBS += -ldwmapi

RC_ICONS = icons/app.ico
