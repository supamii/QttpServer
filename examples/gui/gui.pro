QT += core gui widgets

TARGET = gui
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

message('Including core files')
include($$PWD/../../core.pri)
