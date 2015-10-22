TEMPLATE = app

CONFIG += c++14

QMAKE_CXXFLAGS += -std=gnu++0x -stdlib=libc++ -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

QT += core

HEADERS +=

SOURCES += $$PWD/main.cpp

OBJECTS +=

LIBS += -framework CoreFoundation -framework CoreServices

LIBS += -L$$PWD/../out/Debug -luv -lhttp_parser -lnode_native

PRE_TARGETDEPS += $$PWD/../out/Debug/libuv.a

INCLUDEPATH += \
    $$PWD/../http-parser \
    $$PWD/../libuv/ \
    $$PWD/../libuv/include \
    $$PWD/../include \
    $$PWD/../include/native

DEPENDPATH += $$PWD/../out/Debug


