TEMPLATE = app

QT -= gui

SOURCES += \
    $$PWD/main.cpp

DESTDIR = $$PWD

message('Including config files')
include($$PWD/config/config.pri)

message('Including core files')
include($$PWD/core.pri)
