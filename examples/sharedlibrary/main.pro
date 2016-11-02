QT += core
QT -= gui

TARGET = main
TEMPLATE = app

CONFIG += console QTTP_LIBRARY
CONFIG -= app_bundle

SOURCES += main.cpp

message('Including core files')
include($$PWD/../../core.pri)

LIBS += -L$$PWD
message('LIBS: ' + $$LIBS);

# We'll just throw everything into the project folder
DESTDIR = $$PWD
