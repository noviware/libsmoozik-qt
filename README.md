libsmoozik-qt
=============

A Qt C++ library for the Smoozik API

libsmoozik-qt is a collection of libraries to help you integrate Smoozik services
into your rich desktop software. It is officially supported software developed
by Smoozik staff:

Fabien Pierre-Nicolas: http://twitter.com/FabienPN

Fork it: https://github.com/fabienpn/libsmoozik-qt

# Dependencies

libsmoozik-qt requires:

* Qt 5.0 or Qt 4.8 http://qt.digia.com/
* Doxygen http://www.stack.nl/~dimitri/doxygen/. Required if you want to generate the documentation from source code.
* Gcov http://gcc.gnu.org/onlinedocs/gcc/Gcov.html. Gcov is used for code coverage. If you do not want to use it, be sure to comment it on common.pri

# Using libsmoozik-qt

This C++ API provides equivalent functions to methods available on http://smoozik.com/index.php/api.

You need an API key from http://smoozik.com to access the API.

Linker should include:

    -lqtsmoozik -lQtCore -lQtNetwork -lQtXml

