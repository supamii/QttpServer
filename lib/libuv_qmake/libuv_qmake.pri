INCLUDEPATH += \
    $$PWD/../libuv/src \
    $$PWD/../libuv/include

HEADERS += $$PWD/../libuv/src/*.h \
    $$PWD/../libuv/include/pthread-barrier.h \
    $$PWD/../libuv/include/uv-errno.h \
    $$PWD/../libuv/include/uv-threadpool.h \
    $$PWD/../libuv/include/uv-version.h \
    $$PWD/../libuv/include/uv-common.h \
    $$PWD/../libuv/include/uv.h \
    $$PWD/../libuv/include/tree.h

win32 {
    HEADERS += $$PWD/../libuv/include/uv-win.h

}

unix:!macx {
    HEADERS += $$PWD/../libuv/include/uv-unix.h
}

macx: {
    HEADERS += $$PWD/../libuv/include/uv-darwin.h
}

SOURCES += $$PWD/../libuv/src/*.c

win32 {
    INCLUDEPATH += $$PWD/../libuv/src/win
    HEADERS += $$PWD/../libuv/src/win/*.h
    SOURCES += $$PWD/../libuv/src/win/*.c
}

unix {
    INCLUDEPATH += $$PWD/../libuv/src/unix
    HEADERS += $$PWD/../libuv/src/unix/*.h
    SOURCES += $$PWD/../libuv/src/unix/async.c \
        $$PWD/../libuv/src/unix/core.c \
        $$PWD/../libuv/src/unix/dl.c \
        $$PWD/../libuv/src/unix/fs.c \
        $$PWD/../libuv/src/unix/getaddrinfo.c \
        $$PWD/../libuv/src/unix/getnameinfo.c \
        $$PWD/../libuv/src/unix/loop.c \
        $$PWD/../libuv/src/unix/pipe.c \
        $$PWD/../libuv/src/unix/poll.c \
        $$PWD/../libuv/src/unix/process.c \
        $$PWD/../libuv/src/unix/proctitle.c \
        $$PWD/../libuv/src/unix/signal.c \
        $$PWD/../libuv/src/unix/stream.c \
        $$PWD/../libuv/src/unix/tcp.c \
        $$PWD/../libuv/src/unix/thread.c \
        $$PWD/../libuv/src/unix/timer.c \
        $$PWD/../libuv/src/unix/tty.c \
        $$PWD/../libuv/src/unix/udp.c
}

unix:!macx {
    SOURCES += $$PWD/../libuv/src/unix/linux-core.c \
        $$PWD/../libuv/src/unix/linux-inotify.c \
        $$PWD/../libuv/src/unix/linux-syscalls.c
}

macx {
    SOURCES += $$PWD/../libuv/src/unix/darwin.c \
        $$PWD/../libuv/src/unix/darwin-proctitle.c \
        $$PWD/../libuv/src/unix/fsevents.c \
        $$PWD/../libuv/src/unix/kqueue.c \
        $$PWD/../libuv/src/unix/pthread-barrier.c \
        $$PWD/../libuv/src/unix/pthread-fixes.c \
}
