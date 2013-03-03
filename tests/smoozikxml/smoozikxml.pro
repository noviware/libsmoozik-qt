QT += core testlib network xml
CONFIG += testcase
include(../../common.pri)

INCLUDEPATH += ../../src

CONFIG(release, debug|release) {
    LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozik
    unix:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik.so
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozik.dll
}

CONFIG(debug, debug|release) {
    LIBS += -L$$SMOOZIKLIB_DIR -lqtsmoozikd
    unix:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd.so
    win32-msvc:PRE_TARGETDEPS += $$SMOOZIKLIB_DIR/libqtsmoozikd.dll
}

#Tell the exe to look for shared library in SMOOZIKLIB_DIR
unix:QMAKE_LFLAGS += -Wl,-R -Wl,$$SMOOZIKLIB_DIR

HEADERS += \
    testsmoozikxml.h \
    simplehttpserver.h

SOURCES += \
    testsmoozikxml.cpp \
    simplehttpserver.cpp
