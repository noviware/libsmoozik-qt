/*
   Copyright 2013 Noviware SARL.
      - Primarily authored by Fabien Pierre-Nicolas

   This file is part of libsmoozk-qt.

   libsmoozk-qt is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   libsmoozk-qt is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with libsmoozk-qt.  If not, see <http://www.gnu.org/licenses/>.
 */

/** @file */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtCore/QtGlobal>

/**
 * @mainpage
 * A Qt C++ library for the Smoozik API
 *
 * libsmoozik-qt is a collection of libraries to help you integrate Smoozik services into your rich desktop software. It is officially supported software developed by Smoozik staff:
 *
 * Fabien Pierre-Nicolas: http://twitter.com/FabienPN
 *
 * Fork it: https://github.com/fabienpn/libsmoozik-qt
 * @section dependencies Dependencies
 * libsmoozik-qt requires:
 *
 * Qt 5.0 or Qt 4.8 http://qt.digia.com/
 * Doxygen http://www.stack.nl/~dimitri/doxygen/. Required if you want to generate the documentation from source code.
 * Gcov http://gcc.gnu.org/onlinedocs/gcc/Gcov.html. Gcov is used for code coverage. If you do not want to use it, be sure to comment it on common.pri
 *
 * @section howto Using libsmoozik-qt
 * This C++ API provides equivalent functions to methods available on http://smoozik.com/index.php/api.
 *
 * You need an API key from http://smoozik.com to access the API.
 *
 * Linker should include: -lqtsmoozik -lQtCore -lQtNetwork -lQtXml
 *
 * A typical client program would run a state machine as follows:
 * @image html res/statemachine.png
 *
 */

#if defined(SMOOZIK_LIBRARY)
#  define SMOOZIKLIB_EXPORT Q_DECL_EXPORT
#else
#  define SMOOZIKLIB_EXPORT Q_DECL_IMPORT
#endif

/**
 * @brief This is the max advised size of a playlist for user experience to stay enjoyable on mobile phones.
 *
 * Too long playlists would lag on certain mobile phones.
 */
#define MAX_ADVISED_PLAYLIST_SIZE 200

#endif // GLOBAL_H
