TEMPLATE = app

QT -= gui

DESTDIR = $$PWD

SOURCES += $$PWD/main.cpp

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
    LIBS += \
        -L$$PWD/../../lib/mongo-cxx-driver/build/darwin/64 \
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
        $$PWD/../../lib/mongo-cxx-driver/src \
        $$PWD/../../lib/mongo-cxx-driver/build/darwin/64 \
        /usr/local/include
}

message('Including config files')
include($$PWD/config/config.pri)

message('Including core files')
include($$PWD/../../core.pri)
