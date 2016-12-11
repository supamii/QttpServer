INCLUDEPATH += \
    $$PWD/../libuv/src \
    $$PWD/../libuv/include

HEADERS += \
    $$PWD/../libuv/include/pthread-barrier.h \
    $$PWD/../libuv/include/tree.h \
    $$PWD/../libuv/include/uv-errno.h \
    $$PWD/../libuv/include/uv-threadpool.h \
    $$PWD/../libuv/include/uv-version.h \
    $$PWD/../libuv/src/heap-inl.h \
    $$PWD/../libuv/src/queue.h \
    $$PWD/../libuv/src/uv-common.h

win32 {
    #HEADERS += $$PWD/../libuv/include/uv.h
    #HEADERS += $$PWD/../libuv/include/uv-win.h
}

unix:!macx {
    HEADERS += $$PWD/../libuv/include/uv.h
    HEADERS += $$PWD/../libuv/include/uv-unix.h
}

macx: {
    HEADERS += $$PWD/../libuv/include/uv.h
    HEADERS += $$PWD/../libuv/include/uv-darwin.h
}

win32 {
    contains(CONFIG, MINGW) {
        message("Adding mingw LEAN AND MEAN files")
        DEFINES += WIN32_LEAN_AND_MEAN
    }
    INCLUDEPATH += $$PWD/../libuv/src/win
    HEADERS += $$PWD/../libuv/src/win/*.h
    SOURCES += \
        $$PWD/../libuv/src/win/async.c \
        $$PWD/../libuv/src/win/core.c \
        $$PWD/../libuv/src/win/detect-wakeup.c \
        $$PWD/../libuv/src/win/dl.c \
        $$PWD/../libuv/src/win/error.c \
        $$PWD/../libuv/src/win/fs-event.c \
        $$PWD/../libuv/src/win/fs.c \
        $$PWD/../libuv/src/win/getaddrinfo.c \
        $$PWD/../libuv/src/win/getnameinfo.c \
        $$PWD/../libuv/src/win/handle.c \
        $$PWD/../libuv/src/win/loop-watcher.c \
        $$PWD/../libuv/src/win/pipe.c \
        $$PWD/../libuv/src/win/poll.c \
        $$PWD/../libuv/src/win/process-stdio.c \
        $$PWD/../libuv/src/win/process.c \
        $$PWD/../libuv/src/win/req.c \
        $$PWD/../libuv/src/win/signal.c \
        $$PWD/../libuv/src/win/snprintf.c \
        $$PWD/../libuv/src/win/stream.c \
        $$PWD/../libuv/src/win/tcp.c \
        $$PWD/../libuv/src/win/thread.c \
        $$PWD/../libuv/src/win/timer.c \
        $$PWD/../libuv/src/win/tty.c \
        $$PWD/../libuv/src/win/udp.c \
        $$PWD/../libuv/src/win/util.c \
        $$PWD/../libuv/src/win/winapi.c \
        $$PWD/../libuv/src/win/winsock.c

    SOURCES += \
        $$PWD/../libuv/src/fs-poll.c \
        $$PWD/../libuv/src/inet.c \
        $$PWD/../libuv/src/threadpool.c \
        $$PWD/../libuv/src/uv-common.c \
        $$PWD/../libuv/src/version.c
}

!win32 {
    SOURCES += $$PWD/../libuv/src/*.c
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
        $$PWD/../libuv/src/unix/pthread-fixes.c
}
