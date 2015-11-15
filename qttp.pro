TEMPLATE = app

CONFIG += c++14

QMAKE_CXXFLAGS += -std=gnu++0x -stdlib=libc++ -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

QT += core network

include($$PWD/qttp/qttp.pri)

HEADERS +=

SOURCES += $$PWD/main.cpp

OBJECTS +=

LIBS += -framework CoreFoundation -framework CoreServices

LIBS += -L$$PWD/out/Debug -luv -lhttp_parser -lnode_native

PRE_TARGETDEPS += $$PWD/out/Debug/libuv.a

INCLUDEPATH += \
    $$PWD/http-parser \
    $$PWD/libuv/ \
    $$PWD/libuv/include \
    $$PWD/http/include \
    $$PWD/http/include/native \
    $$PWD/qttp/

DEPENDPATH += $$PWD/out/Debug


