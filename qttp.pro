TEMPLATE = app

QMAKE_CXXFLAGS += -std=gnu++0x -stdlib=libc++ -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

QT += core network

include($$PWD/config/config.pri)
include($$PWD/qttp/qttp.pri)

HEADERS += \
    $$PWD/src/*.h

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/src/*.cpp

OBJECTS +=

LIBS += -luv -lhttp_parser -lnode_native

#LIBS += -mongoclient -L$$PWD/lib/mongo-cxx-driver/build/darwin/normal
#INCLUDEPATH += \
#    $$PWD/lib/mongo-cxx-driver/src \
#    $$PWD/lib/mongo-cxx-driver/build/darwin/normal \
#    /usr/local/opt/boost155/include
#LDFLAGS:  -L/usr/local/opt/boost155/lib
#CPPFLAGS: -I/usr/local/opt/boost155/include

INCLUDEPATH += \
    $$PWD/lib/http-parser \
    $$PWD/lib/libuv/ \
    $$PWD/lib/libuv/include \
    $$PWD/lib/http/include \
    $$PWD/lib/http/include/native \
    $$PWD/qttp

unix: {
}

macx: {
    LIBS += -framework CoreFoundation -framework CoreServices
    CONFIG += c++14
}

unix:!macx {
	CONFIG += c++0x
}

win {
}

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/out/Debug
    DEPENDPATH += $$PWD/out/Debug
    DESTDIR = $$PWD/out/qtdebug
    OBJECT_DIRS = $$PWD/out/qtdebug
    MOC_DIR = $$PWD/out/qtdebug
    RCC_DIR = $$PWD/out/qtdebug
    UI_DIR = $$PWD/out/qtdebug
} else {
    LIBS += -L$$PWD/out/Release
    DEPENDPATH += $$PWD/out/Release
    DESTDIR = $$PWD/out/qtrelease
    OBJECT_DIRS = $$PWD/out/qtrelease
    MOC_DIR = $$PWD/out/qtrelease
    RCC_DIR = $$PWD/out/qtrelease
    UI_DIR = $$PWD/out/qtrelease
}
