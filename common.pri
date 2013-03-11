SMOOZIKLIB_DIR = $$PWD/lib
LIBSMOOZIK_VER = 1

INCLUDEPATH += $$PWD/src

#Code coverage. gcov is required. Comment this if you do not want to use gcov code coverage
unix:CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage -O0
    LIBS += -lgcov
}
