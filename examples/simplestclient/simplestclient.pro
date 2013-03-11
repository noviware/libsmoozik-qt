include(../../common.pri)
QT       += core
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
lessThan(QT_MAJOR_VERSION, 5): QT += gui

TARGET = simplestclient
TEMPLATE = app

CONFIG(release, debug|release) {
    unix:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozik
    unix:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik.so
    win32:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozik$${LIBSMOOZIK_VER}
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik$${LIBSMOOZIK_VER}.dll
}

CONFIG(debug, debug|release) {
    unix:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozikd
    unix:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd.so
    win32:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozikd$${LIBSMOOZIK_VER}
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd$${LIBSMOOZIK_VER}.dll
}

#Tell the exe to look for shared library in SMOOZIKLIB_DIR
unix:QMAKE_LFLAGS += -Wl,-R -Wl,$$SMOOZIKLIB_DIR

SOURCES += main.cpp\
        smooziksimplestclientwindow.cpp

HEADERS  += smooziksimplestclientwindow.h

FORMS    += smooziksimplestclientwindow.ui
