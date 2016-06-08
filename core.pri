QT += core network

message('Including qttp source files')
include($$PWD/src/qttp.pri)

OTHER_FILES += \
    $$PWD/lib/http/include/native/*.h \
    $$PWD/lib/http/src/* \
    $$PWD/lib/http-parser/http_parser.c \
    $$PWD/lib/http-parser/http_parser.h \
    $$PWD/lib/libuv/src/* \
    $$PWD/lib/libuv/include/* \
    $$PWD/*.md

unix {
    OTHER_FILES += $$PWD/lib/libuv/src/unix/*
} else {
    OTHER_FILES += $$PWD/lib/libuv/src/win/*
}

HEADERS +=

SOURCES +=

INCLUDEPATH += \
    $$PWD/lib/http-parser \
    $$PWD/lib/libuv/ \
    $$PWD/lib/libuv/include \
    $$PWD/lib/http/include \
    $$PWD/lib/http/include/native

contains(CONFIG, SSL_TLS) {

    DEFINES += SSL_TLS_UV

    HEADERS += $$PWD/lib/evt_tls/*.h

    SOURCES += \
        $$PWD/lib/evt_tls/uv_tls.c \
        $$PWD/lib/evt_tls/evt_tls.c

    INCLUDEPATH += \
        $$PWD/lib/evt_tls \
        $$PWD/lib

    # Currently this is specifically for mac + homebrew soft links.
    macx: {
        # The user should be able to provide the exact location of openssl.
        INCLUDEPATH += /usr/local/opt/openssl/include
        LIBS += -L/usr/local/opt/openssl/lib -lssl -lcrypto
    }
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
        -lnode_native \
        -ladvapi32 \
        -liphlpapi \
        -lpsapi \
        -lshell32 \
        -lws2_32 \
        -luserenv
} else {
    LIBS += -luv -lhttp_parser -lnode_native
}

contains(CONFIG, QTTP_LIBRARY) {
    LIBS += -lqttpserver
}

CONFIG(debug, debug|release) {
    message('Compiling in DEBUG mode')
    BUILDTYPE = Debug
    QTBUILDTYPE = qtdebug
} else {
    message('Compiling in RELEASE mode')
    BUILDTYPE = Release
    QTBUILDTYPE = qtrelease
}

MOC_DIR = $$PWD/build/$$QTBUILDTYPE
RCC_DIR = $$PWD/build/$$QTBUILDTYPE
UI_DIR = $$PWD/build/$$QTBUILDTYPE

unix:!macx {
    message('Adding http_parser.o on linux')
    # For some reason Ubuntu 12 LTS doesn't jive with only the static lib
    OBJECTS += $$PWD/build/out/$$BUILDTYPE/obj.target/http_parser/lib/http-parser/http_parser.o
}

isEmpty(DESTDIR) {
    DESTDIR = $$PWD/build/$$QTBUILDTYPE
}

win32 {
    LIBS += -L$$PWD/build/$$BUILDTYPE -L$$PWD/build/$$BUILDTYPE/lib
    DEPENDPATH += $$PWD/build/$$BUILDTYPE
    OBJECTS_DIR = $$PWD/build/$$BUILDTYPE/obj
} else {
    LIBS += -L$$PWD/build/out/$$BUILDTYPE
    DEPENDPATH += $$PWD/build/out/$$BUILDTYPE
    OBJECTS_DIR = $$PWD/build/$$QTBUILDTYPE
}

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)
LIBS = $$unique(LIBS)
OBJECTS = $$unique(OBJECTS)
