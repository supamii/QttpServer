message($$PWD)

HEADERS += $$PWD/include/native/*.h

SOURCES += $$PWD/src/*.cc

INCLUDEPATH += \
    $$PWD/include \
    $$PWD/../libuv/include \
    $$PWD/../http-parser \
    $$PWD/../evt_tls

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../build/out/Debug
} else {
    LIBS += -L$$PWD/../../build/out/Release
}

LIBS += -luv -lhttp_parser

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
}

contains(CONFIG, SSL_TLS) {

    DEFINES += SSL_TLS_UV

    HEADERS += $$PWD/../evt_tls/*.h

    SOURCES += \
        $$PWD/../evt_tls/uv_tls.c \
        $$PWD/../evt_tls/evt_tls.c

    INCLUDEPATH += \
        $$PWD/../evt_tls \
        $$PWD/..

    # Currently this is specifically for mac + homebrew soft links.
    macx: {
        # The user should be able to provide the exact location of openssl.
        INCLUDEPATH += /usr/local/opt/openssl/include
        LIBS += -L/usr/local/opt/openssl/lib -lssl -lcrypto
    }
}

# For ease and quick reference in QtCreator
OTHER_FILES += \
    $$PWD/../libuv/include/*.h \
    $$PWD/../http-parser/*.h

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)
