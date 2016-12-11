QT -= gui network

# If the core lib is loaded, then we'll automatically compile the Qt-version of
# the HttpServer.
QT += core

TEMPLATE = lib
CONFIG += staticlib
TARGET = node_native

!win32 {
    VERSION = 0.0.1
}

DEFINES += NNATIVE_ERROR_LOGGING

include($$PWD/http.pri)

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

CONFIG(debug, debug|release) {
    OTHER_FILES += $$PWD/../../build/Debug/lib/*
    OTHER_FILES += $$PWD/../../build/out/Debug/*
} else {
    OTHER_FILES += $$PWD/../../build/Release/lib/*
    OTHER_FILES += $$PWD/../../build/out/Release/*
}

macx: {
    LIBS += -framework CoreFoundation # -framework CoreServices
    CONFIG += c++14
    QMAKE_CXXFLAGS += -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=gnu++0x -stdlib=libc++
}

unix:!macx {
    CONFIG += c++0x
    # This supports GCC 4.7
    QMAKE_CXXFLAGS += -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=c++0x
}

win32 {
    CONFIG += c++14
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

# For ease and quick reference in QtCreator
# OTHER_FILES += \
#     $$PWD/../libuv/include/*.h \
#     $$PWD/../http-parser/*.h

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)
