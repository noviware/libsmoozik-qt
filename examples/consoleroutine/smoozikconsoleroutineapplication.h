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

#ifndef SMOOZIKCONSOLEROUTINEAPPLICATION_H
#define SMOOZIKCONSOLEROUTINEAPPLICATION_H

#include <QCoreApplication>

class SmoozikConsoleRoutineApplication : public QCoreApplication
{
public:
    SmoozikConsoleRoutineApplication(int& argc, char* argv[]);
    int run();
};

#endif // SMOOZIKCONSOLEROUTINEAPPLICATION_H
