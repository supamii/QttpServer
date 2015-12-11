TEMPLATE = app

QT += core network

message('Including config files')
include($$PWD/config/config.pri)

message('Including qttp source files')
include($$PWD/qttp/qttp.pri)

HEADERS +=

SOURCES += \
    $$PWD/main.cpp

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
    $$PWD/lib/http/include/native

unix: {
    QMAKE_CXXFLAGS += -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
}

macx: {
    LIBS += -framework CoreFoundation -framework CoreServices
    CONFIG += c++14
    QMAKE_CXXFLAGS += -std=gnu++0x -stdlib=libc++
}

unix:!macx {
    CONFIG += c++0x
    # This supports GCC 4.7
    QMAKE_CXXFLAGS += -std=c++0x
}

win32 {
    CONFIG += c++14
    QMAKE_CXXFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
}

win32 {
    LIBS += \
        -llibuv \
        -lhttp_parser \
        -lnode_native \
        -ladvapi32 \
        -liphlpapi \
        -lpsapi \
        -lshell32 \
        -lws2_32 \
        -luserenv
}

!win32 {
    LIBS += -luv -lhttp_parser -lnode_native
}

CONFIG(debug, debug|release) {
    message('Compiling in DEBUG mode')
    LIBS += -L$$PWD/out/Debug
    DEPENDPATH += $$PWD/out/Debug
    DESTDIR = $$PWD/out/qtdebug
    OBJECTS_DIR = $$PWD/out/qtdebug
    MOC_DIR = $$PWD/out/qtdebug
    RCC_DIR = $$PWD/out/qtdebug
    UI_DIR = $$PWD/out/qtdebug
    unix:!macx {
        message('Adding http_parser.o on linux')
        # For some reason Ubuntu 12 LTS doesn't jive with only the static lib
        OBJECTS += $$PWD/out/Debug/obj.target/http_parser/lib/http-parser/http_parser.o
    }
} else {
    message('Compiling in RELEASE mode')
    LIBS += -L$$PWD/out/Release
    DEPENDPATH += $$PWD/out/Release
    DESTDIR = $$PWD/out/qtrelease
    OBJECTS_DIR = $$PWD/out/qtrelease
    MOC_DIR = $$PWD/out/qtrelease
    RCC_DIR = $$PWD/out/qtrelease
    UI_DIR = $$PWD/out/qtrelease
    unix:!macx {
        message('Adding http_parser.o on linux')
        OBJECTS += $$PWD/out/Release/obj.target/http_parser/lib/http-parser/http_parser.o
    }
}

win32 {
    #QMAKE_CFLAGS_RELEASE += -MT
    #QMAKE_CXXFLAGS_RELEASE += -MT
    #QMAKE_CFLAGS_RELEASE -= -MD
    #QMAKE_CXXFLAGS_RELEASE -= -MD

    #QMAKE_CFLAGS_DEBUG += -MTd
    #QMAKE_CXXFLAGS_DEBUG += -MTd
    #QMAKE_CFLAGS_DEBUG -= -MDd
    #QMAKE_CXXFLAGS_DEBUG -= -MDd
}

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)
LIBS = $$unique(LIBS)
OBJECTS = $$unique(OBJECTS)
