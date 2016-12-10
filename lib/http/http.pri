message($$PWD)

HEADERS += \
    $$PWD/include/native/base.h \
    $$PWD/include/native/callback.h \
    $$PWD/include/native/error.h \
    $$PWD/include/native/fs.h \
    $$PWD/include/native/handle.h \
    $$PWD/include/native/http.h \
    $$PWD/include/native/loop.h \
    $$PWD/include/native/native.h \
    $$PWD/include/native/net.h \
    $$PWD/include/native/stream.h \
    $$PWD/include/native/tcp.h \
    $$PWD/include/native/text.h

SOURCES += \
    $$PWD/src/fs.cc \
    $$PWD/src/handle.cc \
    $$PWD/src/http.cc \
    $$PWD/src/loop.cc \
    $$PWD/src/net.cc \
    $$PWD/src/stream.cc \
    $$PWD/src/tcp.cc

INCLUDEPATH += \
    $$PWD/include \
    $$PWD/include/native \
    $$PWD/../libuv/include \
    $$PWD/../http-parser \
    $$PWD/../evt_tls

CONFIG(debug, debug|release) {
    LIBS += -L$$PWD/../../build/out/Debug
} else {
    LIBS += -L$$PWD/../../build/out/Release
}

LIBS += -luv -lhttp_parser

contains(QT, core) {
  HEADERS += $$PWD/qttp/*.h
  SOURCES += $$PWD/qttp/*.cc
  INCLUDEPATH += $$PWD/qttp
}

contains(CONFIG, MINGW) {
    DEFINES += NNATIVE_EXPORT
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
        message('Adding openSSL libraries')
        # The user should be able to provide the exact location of openssl.
        INCLUDEPATH += /usr/local/opt/openssl/include
        LIBS += -L/usr/local/opt/openssl/lib -lssl -lcrypto
    }
}
