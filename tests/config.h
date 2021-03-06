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
#ifndef CONFIG_H
#define CONFIG_H

#ifndef MANAGER_USERNAME
/**
 * @brief Username used to test manager functions in unit tests.
 */
#define MANAGER_USERNAME ""
#endif // MANAGER_USERNAME

#ifndef MANAGER_PASSWORD
/**
 * @brief Password used to test manager functions in unit tests.
 */
#define MANAGER_PASSWORD ""
#endif // MANAGER_PASSWORD

#ifndef MEMBER_USERNAME
/**
 * @brief Username used to test member functions in unit tests.
 */
#define MEMBER_USERNAME ""
#endif // MEMBER_USERNAME

#ifndef MEMBER_PASSWORD
/**
 * @brief Password used to test member functions in unit tests.
 */
#define MEMBER_PASSWORD ""
#endif // MEMBER_PASSWORD

#ifndef APIKEY
/**
 * @brief API key used to test functions in unit tests.
 */
#define APIKEY ""
#endif // APIKEY

#ifndef SECRET
/**
 * @brief Secret used to test functions in unit tests.
 */
#define SECRET ""
#endif // SECRET

/**
 * @brief Main application running a test case and generating an xml output
 */
#define QTEST_XML_MAIN(TestObject) \
int main(int argc, char *argv[]) \
{ \
    QCoreApplication app(argc, argv); \
    TestObject tc; \
    QStringList testCmd; \
    QDir testLogDir; \
    testLogDir.mkdir("test-reports"); \
    testCmd<<" "<<"-xml" <<"-o" <<"test-reports/results.xml"; \
    return QTest::qExec(&tc,testCmd); \
}

#endif // CONFIG_H
