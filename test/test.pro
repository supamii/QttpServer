TEMPLATE = app

QT += testlib
QT -= gui

HEADERS += \
    sampleAction.h

SOURCES += \
    testHttpServer.cpp

DESTDIR = $$PWD

# Skip timestamps since it adds clutter to unit test logs.
DEFINES += NO_LOG_DATETIME

macx {
    # Since things are buried in the app folder, we'll copy configs there.
    MediaFiles.files = \
        $$PWD/config/global.json \
        $$PWD/config/routes.json

    MediaFiles.path = Contents/MacOS/config
    QMAKE_BUNDLE_DATA += MediaFiles
}

message('Including config files')
include($$PWD/config/config.pri)

message('Including core files')
include($$PWD/../core.pri)
