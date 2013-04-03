TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src \
    tests \
    3rdparty \
    demos
tests.depends = src
demos.depends = src
demos.depends = 3rdparty
