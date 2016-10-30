QT += core network

# Good for dev/test but in prod let's skip this all together.
DEFINES += QTTP_OMIT_ASSERTIONS

CONFIG(debug, debug|release) {
    message('Compiling in DEBUG mode')
    BUILDTYPE = Debug
    QTBUILDTYPE = qtdebug
} else {
    message('Compiling in RELEASE mode')
    BUILDTYPE = Release
    QTBUILDTYPE = qtrelease
}

# For some reason Ubuntu 12 LTS doesn't jive with only the static lib!
#
# This isn't an issue on TravisCI with Ubuntu14 so let's make this configurable.

contains(CONFIG, HTTP_PARSER_WORKAROUND) {
  unix:!macx {
      message('Adding http_parser.o on linux')
      OBJECTS += $$PWD/build/out/$$BUILDTYPE/obj.target/http_parser/lib/http-parser/http_parser.o
  }
}

message('Including qttp source files')
include($$PWD/src/qttp.pri)

OTHER_FILES += \
    $$PWD/lib/http/include/native/*.h \
    $$PWD/lib/http/src/* \
    $$PWD/lib/http-parser/http_parser.c \
    $$PWD/lib/http-parser/http_parser.h \
    $$PWD/lib/libuv/src/* \
    $$PWD/lib/libuv/include/* \
    $$PWD/*.md

unix {
    OTHER_FILES += $$PWD/lib/libuv/src/unix/*
} else {
    OTHER_FILES += $$PWD/lib/libuv/src/win/*
}

HEADERS +=

SOURCES +=

INCLUDEPATH += \
    $$PWD/lib/http-parser \
    $$PWD/lib/libuv/ \
    $$PWD/lib/libuv/include \
    $$PWD/lib/http/include \
    $$PWD/lib/http/include/native \
    $$PWD/lib/http/qttp

contains(CONFIG, SSL_TLS) {

    DEFINES += SSL_TLS_UV

    # HEADERS += $$PWD/lib/evt_tls/*.h
    # SOURCES += $$PWD/lib/evt_tls/uv_tls.c $$PWD/lib/evt_tls/evt_tls.c

    INCLUDEPATH += \
        $$PWD/lib/evt_tls \
        $$PWD/lib

    # Currently this is specifically for mac + homebrew soft links.
    macx: {
        message('Adding openSSL libraries')
        # The user should be able to provide the exact location of openssl.
        INCLUDEPATH += /usr/local/opt/openssl/include
        LIBS += -L/usr/local/opt/openssl/lib -lssl -lcrypto
    }
}

macx: {
    LIBS += -framework CoreFoundation # -framework CoreServices
    CONFIG += c++14
    QMAKE_CXXFLAGS += -g -O0 -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=gnu++0x -stdlib=libc++
}

unix:!macx {
    CONFIG += c++0x
    # This supports GCC 4.7
    QMAKE_CXXFLAGS += -g -O0 -lm -lpthread -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -std=c++0x
}

win32 {
    CONFIG += c++14
    QMAKE_CXXFLAGS += -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
    LIBS += \
        -ladvapi32 \
        -liphlpapi \
        -lpsapi \
        -lshell32 \
        -lws2_32 \
        -luserenv \
        -luser32
}

# ARG order matters here, always make sure node_native goes first!
LIBS += -lnode_native -luv -lhttp_parser

contains(TEMPLATE, lib) {
    message('Building QTTP library')
    contains(CONFIG, staticlib) {
        message('Building staticlib')
    } else {
        message('Building shared library')
        DEFINES += QTTP_EXPORT
    }
} else {
    contains(CONFIG, QTTP_LIBRARY) {
        message('Including QTTP library')
        LIBS += -lqttpserver
    }
}

MOC_DIR = $$PWD/build/$$QTBUILDTYPE
RCC_DIR = $$PWD/build/$$QTBUILDTYPE
UI_DIR = $$PWD/build/$$QTBUILDTYPE

isEmpty(DESTDIR) {
    DESTDIR = $$PWD/build/$$QTBUILDTYPE
}

win32 {
    LIBS += -L$$PWD/build/$$BUILDTYPE -L$$PWD/build/$$BUILDTYPE/lib
    DEPENDPATH += $$PWD/build/$$BUILDTYPE
    OBJECTS_DIR = $$PWD/build/$$BUILDTYPE/obj
} else {
    LIBS += -L$$PWD/build/out/$$BUILDTYPE
    DEPENDPATH += $$PWD/build/out/$$BUILDTYPE
    OBJECTS_DIR = $$PWD/build/$$QTBUILDTYPE
}

macx {
    # Since things are buried in the app folder, we'll copy configs there.

    Web.files = $$PWD/www/swagger-ui.js $$PWD/www/swagger-ui.min.js \
        $$PWD/www/index.html $$PWD/www/o2c.html
    Web.path = Contents/MacOS/www

    Css.files = $$PWD/www/css/print.css $$PWD/www/css/reset.css \
        $$PWD/www/css/screen.css $$PWD/www/css/style.css \
        $$PWD/www/css/typography.css $$PWD/www/css/theme-flattop.css \
        $$PWD/www/css/theme-feeling-blue.css $$PWD/www/css/theme-monokai.css \
        $$PWD/www/css/theme-muted.css $$PWD/www/css/theme-newspaper.css \
        $$PWD/www/css/theme-outline.css
    Css.path = Contents/MacOS/www/css

    Fonts.files = $$PWD/www/fonts/DroidSans-Bold.ttf \
        $$PWD/www/fonts/DroidSans.ttf
    Fonts.path = Contents/MacOS/www/fonts

    Images.files = $$PWD/www/images/collapse.gif \
        $$PWD/www/images/expand.gif \
        $$PWD/www/images/explorer_icons.png \
        $$PWD/www/images/favicon-16x16.png \
        $$PWD/www/images/favicon-32x32.png \
        $$PWD/www/images/favicon.ico \
        $$PWD/www/images/logo_small.png \
        $$PWD/www/images/pet_store_api.png \
        $$PWD/www/images/throbber.gif \
        $$PWD/www/images/wordnik_api.png
    Images.path = Contents/MacOS/www/images

    Lang.files = $$PWD/www/lang/ca.js $$PWD/www/lang/en.js \
        $$PWD/www/lang/es.js $$PWD/www/lang/fr.js \
        $$PWD/www/lang/geo.js $$PWD/www/lang/it.js \
        $$PWD/www/lang/ja.js $$PWD/www/lang/ko-kr.js \
        $$PWD/www/lang/pl.js $$PWD/www/lang/pt.js \
        $$PWD/www/lang/ru.js $$PWD/www/lang/tr.js \
        $$PWD/www/lang/translator.js $$PWD/www/lang/zh-cn.js
    Lang.path = Contents/MacOS/www/lang

    JsLib.files = $$PWD/www/lib/backbone-min.js \
        $$PWD/www/lib/es5-shim.js \
        $$PWD/www/lib/handlebars-4.0.5.js \
        $$PWD/www/lib/highlight.9.1.0.pack.js \
        $$PWD/www/lib/highlight.9.1.0.pack_extended.js \
        $$PWD/www/lib/jquery-1.8.0.min.js \
        $$PWD/www/lib/jquery.ba-bbq.min.js \
        $$PWD/www/lib/jquery.slideto.min.js \
        $$PWD/www/lib/jquery.wiggle.min.js \
        $$PWD/www/lib/js-yaml.min.js \
        $$PWD/www/lib/jsoneditor.min.js \
        $$PWD/www/lib/lodash.min.js \
        $$PWD/www/lib/marked.js \
        $$PWD/www/lib/object-assign-pollyfill.js \
        $$PWD/www/lib/sanitize-html.min.js \
        $$PWD/www/lib/swagger-oauth.js
    JsLib.path = Contents/MacOS/www/lib

    QMAKE_BUNDLE_DATA += Web Css Fonts Images JsLib
}

INCLUDEPATH = $$unique(INCLUDEPATH)
HEADERS = $$unique(HEADERS)
SOURCES = $$unique(SOURCES)
LIBS = $$unique(LIBS)
OBJECTS = $$unique(OBJECTS)
