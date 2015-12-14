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

message('Including config files')
include($$PWD/config/config.pri)

message('Including core files')
include($$PWD/../../core.pri)
