TEMPLATE = app

QT -= gui
DESTDIR = $$PWD
SOURCES += $$PWD/main.cpp
TARGET = MongoApp

win32 {
    LIBS += \
        -LC:/local\mongo-client/lib \
        -llibmongoclient-gd \
        -LC:/local/boost_1_59_0/lib32-msvc-14.0

    INCLUDEPATH += \
        C:/local/mongo-client/include \
        C:/local/boost_1_59_0
}

macx {
    # If you don't want the whole mac application folder, comment the below.
    # CONFIG -= app_bundle

    LIBS += \
        -L$$PWD/lib/mongo-cxx-driver/build/install \
        -lmongoclient \
        -L/usr/local/lib \
        -lboost_system \
        -lboost_atomic-mt \
        -lboost_exception \
        -lboost_iostreams \
        -lboost_filesystem \
        -lboost_thread-mt \
        -lboost_regex \
        -lboost_regex-mt

    INCLUDEPATH += \
        $$PWD/lib/mongo-cxx-driver/src \
        $$PWD/lib/mongo-cxx-driver/build/install \
        /usr/local/include
}

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
