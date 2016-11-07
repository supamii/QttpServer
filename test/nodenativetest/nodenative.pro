TEMPLATE = app

QT += testlib
QT -= gui

HEADERS += \
    testHttpServer.h

SOURCES += \
    testHttpServer.cpp

DESTDIR = $$PWD

# Skip timestamps since it adds clutter to unit test logs.
DEFINES += QTTP_OMIT_LOG_DATETIME QTTP_FORMAT_JSON_RESPONSE

macx {
    # Since things are buried in the app folder, we'll copy configs there.
    MediaFiles.files = \
        $$PWD/config/global.json \
        $$PWD/config/routes.json

    MediaFiles.path = Contents/MacOS/config
    QMAKE_BUNDLE_DATA += MediaFiles
}

message('Including core files')
include($$PWD/../../core.pri)
