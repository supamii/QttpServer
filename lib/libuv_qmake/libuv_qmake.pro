QT -= gui core network

TEMPLATE = lib
CONFIG += staticlib

VERSION = 1.10.0
TARGET = uv

include($$PWD/libuv_qmake.pri)

CONFIG(debug, debug|release) {
    win32 {
        DESTDIR = $$PWD/../../build/Debug/lib
        # OTHER_FILES += $$PWD/../../build/Debug/lib/*
    } else {
        DESTDIR = $$PWD/../../build/out/Debug
        # OTHER_FILES += $$PWD/../../build/out/Debug/*
    }
} else {
    win32 {
        DESTDIR = $$PWD/../../build/Release/lib
        # OTHER_FILES += $$PWD/../../build/Release/lib
    } else {
        DESTDIR = $$PWD/../../build/out/Release
        # OTHER_FILES += $$PWD/../../build/out/Release
    }
}

macx: {
    LIBS += -framework CoreFoundation # -framework CoreServices
    # CONFIG += c++14
    QMAKE_CXXFLAGS += -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=gnu++0x -stdlib=libc++
}

unix:!macx {
    # CONFIG += c++0x
    # This supports GCC 4.7
    QMAKE_CXXFLAGS += -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=c++0x
}

win32 {
    # CONFIG += c++14
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

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)

# OTHER_FILES += $$PWD/../libuv/include/*.h $$PWD/../libuv/src/unix/*.c
# OTHER_FILES += $$PWD/../libuv/test/*
