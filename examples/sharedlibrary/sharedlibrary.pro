QT += core
QT -= gui

TARGET = qttpserver
CONFIG += QTTP_SHARED_LIBRARY
TEMPLATE = lib

message('Including core files')
include($$PWD/../../core.pri)

message('LIBS: ' + $$LIBS);

# We'll just throw everything into the project folder
DESTDIR = $$PWD
