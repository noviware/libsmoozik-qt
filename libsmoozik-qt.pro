TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src \
    tests \
    3rdparty \
    examples
tests.depends = src
examples.depends = src
examples.depends = 3rdparty
