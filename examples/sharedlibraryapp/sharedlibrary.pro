QT += core
QT -= gui

CONFIG += c++11

TARGET = SharedLibrary
CONFIG += console QTTP_LIBRARY
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp

message('Including core files')
include($$PWD/../../core.pri)

message('LIBS: ' + $$LIBS);

