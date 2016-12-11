include($$PWD/config/config.pri)
include($$PWD/../framework/framework.pri)

contains(CONFIG, mingw) {
    CONFIG(debug, debug|release) {
        message('Compiling in DEBUG mode')
        BUILDTYPE = Debug
        QTBUILDTYPE = qtdebug
    } else {
        message('Compiling in RELEASE mode')
        BUILDTYPE = Release
        QTBUILDTYPE = qtrelease
    }
    # TODO: Seeing some pretty buggy behaviors from MINGW QMake
    LIBS += -L$$PWD/../../build/$$BUILDTYPE/lib/libqttpserver.a
    DEFINES += NNATIVE_IMPORT
}

DESTDIR = $$PWD

SOURCES += qttptest.cpp
HEADERS += qttptest.h

# Uncomment this whenever we need to debug tests.  Otherwise keep things quiet.
DEFINES += QTTP_DISABLE_LOGGING
