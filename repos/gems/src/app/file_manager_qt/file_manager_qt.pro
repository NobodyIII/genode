#-------------------------------------------------
#
# Project created by QtCreator 2016-01-15T18:03:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = file_manager_qt
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
    mainwindow.cpp \
    genodefsmodel.cpp \
    fsnode.cpp \
    genodefs.cpp \
    fsregistry.cpp \
    test_linux.cpp \
    folderview.cpp \
    dragdropmenu.cpp \
    nodedelegate.cpp

HEADERS  += mainwindow.h \
    genodefsmodel.h \
    fsnode.h \
    genodefs.h \
    fsregistry.h \
    test_linux.h \
    folderview.h \
    dragdropmenu.h \
    nodedelegate.h

FORMS    += mainwindow.ui

RESOURCES += \
    resources.qrc
