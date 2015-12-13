contains(CONFIG, SAMPLEAPP) {
    # This default configuration is for example purposes.
    TEMPLATE = app
    SOURCES += $$PWD/test/sample/main.cpp
    message('Including config files')
    include($$PWD/config/config.pri)
} else {
    TEMPLATE = lib
    CONFIG += staticlib
    VERSION = 0.0.1
}

QT -= gui
TARGET = qttpserver
DESTDIR = $$PWD

message('Including core files')
include($$PWD/core.pri)
