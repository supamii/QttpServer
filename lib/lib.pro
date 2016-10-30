TEMPLATE = subdirs

# Normally this should be commented-out since it can get noisey.
DEFINES += NNATIVE_DEBUG_LOGGING NNATIVE_STD_LOGGING

SUBDIRS += \
    $$PWD/http_parser_qmake \
    $$PWD/libuv_qmake \
    $$PWD/http

CONFIG += ordered
