TEMPLATE = app

#LDFLAGS:  -L/usr/local/opt/boost155/lib
#CPPFLAGS: -I/usr/local/opt/boost155/include

CONFIG += c++14

QMAKE_CXXFLAGS += -std=gnu++0x -stdlib=libc++ -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

QT += core network

include($$PWD/qttp/qttp.pri)
include($$PWD/config/config.pri)

HEADERS +=

SOURCES += $$PWD/main.cpp

OBJECTS +=

LIBS += -luv -lhttp_parser -lnode_native

INCLUDEPATH += \
    $$PWD/lib/http-parser \
    $$PWD/lib/libuv/ \
    $$PWD/lib/libuv/include \
    $$PWD/lib/http/include \
    $$PWD/lib/http/include/native \
    $$PWD/qttp/

unix:!macx {

}

macx: {
    LIBS += -framework CoreFoundation -framework CoreServices
}

win {

}

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/out/Debug
    PRE_TARGETDEPS += $$PWD/out/Debug/libuv.a
    DEPENDPATH += $$PWD/out/Debug
    DESTDIR = $$PWD/out/qtdebug
} else {
    LIBS += -L$$PWD/out/Release
    PRE_TARGETDEPS += $$PWD/out/Release/libuv.a
    DEPENDPATH += $$PWD/out/Release
    DESTDIR = $$PWD/out/qtrelease
}
