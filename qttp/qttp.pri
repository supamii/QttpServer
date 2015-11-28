HEADERS += $$PWD/*.h \
    $$PWD/processor.h
SOURCES += $$PWD/*.cpp
INCLUDEPATH += $$PWD

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)
