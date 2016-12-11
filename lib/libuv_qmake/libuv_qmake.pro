QT -= gui core network

TEMPLATE = lib
CONFIG += staticlib
TARGET = uv

!win32 {
    VERSION = 1.10.0
}

include($$PWD/libuv_qmake.pri)

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

macx: {
    LIBS += -framework CoreFoundation # -framework CoreServices
    QMAKE_CXXFLAGS += -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=gnu++0x -stdlib=libc++
}

unix:!macx {
    # This supports GCC 4.7
    QMAKE_CXXFLAGS += -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=c++0x
}

win32 {
    QMAKE_CXXFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
    LIBS += \
        -llibuv \
        -lhttp_parser \
        -ladvapi32 \
        -liphlpapi \
        -lpsapi \
        -lshell32 \
        -lws2_32 \
        -luserenv \
        -luser32
}

# Might need to go back and clean up the actual list of included source and
# headers because it looks like we've included some test functions, so in the
# meanwhile we can go ahead and stub them by adding stubs.

SOURCES += $$PWD/workaround.c

win32 {
    # Terrible work-around to include "winapi.h" before "winsock2.h"
    INCLUDEPATH += $$PWD
    HEADERS += $$PWD/uv-win.h $$PWD/uv.h
}

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)

# OTHER_FILES += $$PWD/../libuv/include/*.h $$PWD/../libuv/src/unix/*.c
# OTHER_FILES += $$PWD/../libuv/test/*
