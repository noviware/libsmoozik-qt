include(../../common.pri)
include(../../3rdparty/taglib.pri)

QT += core network xml
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets multimedia
lessThan(QT_MAJOR_VERSION, 5): QT += gui phonon

TARGET = simplestclient
TEMPLATE = app

CONFIG(release, debug|release) {
    LIBS += -L$$SMOOZIKLIB_DIR -ltaglib
    linux-g++:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozik
    linux-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik.so \
        $$SMOOZIKLIB_DIR/libtaglib.a
    win32:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozik$${LIBSMOOZIK_VER}
    win32-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik$${LIBSMOOZIK_VER}.a
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik$${LIBSMOOZIK_VER}.dll \
        $$SMOOZIKLIB_DIR/libtaglib.lib
}

CONFIG(debug, debug|release) {
    LIBS += -L$$SMOOZIKLIB_DIR -ltaglibd
    linux-g++:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozikd
    linux-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd.so \
        $$SMOOZIKLIB_DIR/libtaglibd.a
    win32:LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozikd$${LIBSMOOZIK_VER}
    win32-g++:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd$${LIBSMOOZIK_VER}.a
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd$${LIBSMOOZIK_VER}.dll \
        $$SMOOZIKLIB_DIR/libtaglibd.lib
}

#Tell the exe to look for shared library in SMOOZIKLIB_DIR
unix:QMAKE_LFLAGS += -Wl,-R -Wl,$$SMOOZIKLIB_DIR

SOURCES += main.cpp\
    smooziksimplestclientwindow.cpp \
    smoozikplaylistfiller.cpp

HEADERS  += smooziksimplestclientwindow.h \
    config.h \
    smoozikplaylistfiller.h

FORMS    += smooziksimplestclientwindow.ui
