QT += testlib
QT -= gui

HEADERS += \
    sampleAction.h

SOURCES += \
    testHttpServer.cpp

include($$PWD/../server.pri)
