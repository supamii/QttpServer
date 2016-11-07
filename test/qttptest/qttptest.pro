include($$PWD/config/config.pri)
include($$PWD/../framework/framework.pri)

DESTDIR = $$PWD

SOURCES += qttptest.cpp
HEADERS += qttptest.h

# Uncomment this whenever we need to debug tests.  Otherwise keep things quiet.
DEFINES += QTTP_DISABLE_LOGGING
