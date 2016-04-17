contains(CONFIG, SAMPLEAPP) {
    message(********* Building SAMPLE APPLICATION qttpserver *********)
    # This default configuration is for example purposes.
    TEMPLATE = app
    DESTDIR = $$PWD
    SOURCES += $$PWD/examples/sampleapp/main.cpp
    TARGET = QttpServer
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

} else {

    TEMPLATE = lib
    VERSION = 0.0.1
    TARGET = qttpserver

    contains(CONFIG, QTTP_LIBRARY) {
        message(********* Building shared library qttpserver *********)

        DEFINES += QTTP_LIBRARY
        unix {
            #target.path = /usr/lib
            #INSTALLS += target
        }

    } else {
        message(********* Building static library qttpserver *********)
        CONFIG += staticlib
    }

    CONFIG(debug, debug|release) {
        win32 {
            DESTDIR = $$PWD/build/Debug/lib
        } else {
            DESTDIR = $$PWD/build/out/Debug
        }
    } else {
        win32 {
            DESTDIR = $$PWD/build/Release/lib
        } else {
            DESTDIR = $$PWD/build/out/Release
        }
    }
}

QT -= gui

message('Including core files')
include($$PWD/core.pri)
