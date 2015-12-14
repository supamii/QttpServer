TEMPLATE = app

QT -= gui

DESTDIR = $$PWD

SOURCES += $$PWD/main.cpp

message('Including config files')
include($$PWD/config/config.pri)

message('Including core files')
include($$PWD/../../core.pri)
