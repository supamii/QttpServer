contains(CONFIG, SAMPLEAPP) {
	message(********* Building SAMPLE APPLICATION qttpserver *********)
    # This default configuration is for example purposes.
    TEMPLATE = app
	DESTDIR = $$PWD
    SOURCES += $$PWD/test/sample/main.cpp
    message('Including config files')
    include($$PWD/config/config.pri)
} else {
	message(********* Building static library qttpserver *********)
    TEMPLATE = lib
    CONFIG += staticlib
    VERSION = 0.0.1
	CONFIG(debug, debug|release) {
		DESTDIR = $$PWD/build/Debug
	} else {
		DESTDIR = $$PWD/build/Release
	}
}

QT -= gui
TARGET = qttpserver

message('Including core files')
include($$PWD/core.pri)
