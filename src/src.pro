TEMPLATE = lib
QT += core network xml
CONFIG += qt dll
mac:CONFIG += absolute_library_soname
include(../common.pri)
DESTDIR = $$SMOOZIKLIB_DIR
CONFIG(release, debug|release): TARGET = qtsmoozik
CONFIG(debug, debug|release): TARGET = qtsmoozikd

VER_MAJ = 1
VER_MIN = 0
VER_PAT = 0
VERSION = 1.0.0-PR1

HEADERS += \
    smoozikmanager.h \
    smoozikxml.h

SOURCES += \
    smoozikmanager.cpp \
    smoozikxml.cpp
