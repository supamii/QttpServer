QT += testlib
QT -= gui

TEMPLATE = app
DESTDIR = $$PWD
CONFIG -= app_bundle
INCLUDEPATH += $$PWD

# Skip timestamps since it adds clutter to unit test logs.
DEFINES += QTTP_OMIT_LOG_DATETIME QTTP_FORMAT_JSON_RESPONSE
DEFINES += QTTP_ALL_MEMBERS_PUBLIC

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

HEADERS += \
    $$PWD/testutils.h
