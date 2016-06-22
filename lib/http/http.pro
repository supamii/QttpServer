QT -= core network gui

TEMPLATE = lib
CONFIG += staticlib

VERSION = 0.0.1
TARGET = node_native

CONFIG(debug, debug|release) {
    win32 {
        DESTDIR = $$PWD/../../build/Debug/lib
    } else {
        DESTDIR = $$PWD/../../build/out/Debug
    }
} else {
    win32 {
        DESTDIR = $$PWD/../../build/Release/lib
    } else {
        DESTDIR = $$PWD/../../build/out/Release
    }
}

include($$PWD/http.pri)
