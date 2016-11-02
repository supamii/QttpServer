TEMPLATE = app

QT -= gui
DESTDIR = $$PWD

TARGET = computers

SOURCES += \
    main.cpp

HEADERS += \
    phones.h \
    desktops.h \
    tablets.h \
    laptops.h

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
include($$PWD/../../core.pri)


