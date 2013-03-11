TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src \
    tests \
    examples
tests.depends = src
examples.depends = src
