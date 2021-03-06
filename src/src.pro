TEMPLATE = lib
QT += core network xml
CONFIG += qt dll
mac:CONFIG += absolute_library_soname
include(../common.pri)
DESTDIR = $$SMOOZIKLIB_DIR
CONFIG(release, debug|release): TARGET = qtsmoozik
CONFIG(debug, debug|release): TARGET = qtsmoozikd
DEFINES += SMOOZIK_LIBRARY

VER_MAJ = $$LIBSMOOZIK_VER
VER_MIN = 0
VER_PAT = 0
VERSION = 1.0.0

HEADERS += \
    smoozikmanager.h \
    smoozikxml.h \
    global.h \
    smooziktrack.h \
    smoozikplaylist.h

SOURCES += \
    smoozikmanager.cpp \
    smoozikxml.cpp \
    smooziktrack.cpp \
    smoozikplaylist.cpp

#Code coverage. gcov is required. Comment this if you do not want to use gcov code coverage
linux-g++:CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -O0
    LIBS += -lgcov
}
