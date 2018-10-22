TEMPLATE     = app
TARGET       = qttoolbutton_demo

CONFIG      += c++11

QT          += core
QT          += gui
QT          += widgets

SOURCES     += button.cc
SOURCES     += main.cc
SOURCES     += main_window.cc

HEADERS     += button.h
HEADERS     += main_window.h

FORMS       += main_window.ui
