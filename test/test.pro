TEMPLATE = app

QT += testlib
QT -= gui

HEADERS += \
    sampleAction.h

SOURCES += \
    testHttpServer.cpp

DESTDIR = $$PWD

message('Including config files')
include($$PWD/config/config.pri)

message('Including core files')
include($$PWD/../core.pri)
