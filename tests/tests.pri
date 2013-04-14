QT += core testlib network xml
QT -= gui
CONFIG += testcase
include(../common.pri)

CONFIG(release, debug|release) {
    linux-g++:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozik
    linux-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik.so
    win32:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozik$${LIBSMOOZIK_VER}
    win32-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik$${LIBSMOOZIK_VER}.a
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik$${LIBSMOOZIK_VER}.dll
}

CONFIG(debug, debug|release) {
    linux-g++:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozikd
    linux-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd.so
    win32:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozikd$${LIBSMOOZIK_VER}
    win32-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd$${LIBSMOOZIK_VER}.a
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd$${LIBSMOOZIK_VER}.dll
}

#Tell the exe to look for shared library in SMOOZIKLIB_DIR
unix:QMAKE_LFLAGS += -Wl,-R -Wl,$$SMOOZIKLIB_DIR

INCLUDEPATH += $$PWD
