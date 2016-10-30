message($$PWD)

HEADERS += $$PWD/include/native/*.h

SOURCES += $$PWD/src/*.cc

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
