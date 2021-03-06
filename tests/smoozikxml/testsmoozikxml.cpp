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

#include "testsmoozikxml.h"
#include "smoozikxml.h"
#include "simplehttpserver.h"

void TestSmoozikXml::serverUnreachable()
{
    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("invalidurl"));
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply = manager.get(request);
    loop.exec();

    SmoozikXml *xml = new SmoozikXml(reply, this);
    QCOMPARE(xml->error(), SmoozikManager::ServerUnreachable);
    QCOMPARE(xml->errorMsg(), tr("Could not reach server."));
}

void TestSmoozikXml::parse_data()
{
    QTest::addColumn<QString>("response");
    QTest::addColumn<bool>("parseResult");
    QTest::addColumn<int>("errorResult");
    QTest::addColumn<QString>("errorMsgResult");

    QTest::newRow("Empty response") << QString() << false << (int) SmoozikManager::ParseError << "Could not parse xml";
    QTest::newRow("No smoozik") << "<xml></xml>" << false << (int) SmoozikManager::ParseError << "Could not parse xml";
    QTest::newRow("No status") << "<smoozik></smoozik>" << false << (int) SmoozikManager::ParseError << "Could not parse xml";
    QTest::newRow("No error") << "<smoozik><status>failed</status></smoozik>" << false << (int) SmoozikManager::ParseError << "Could not parse xml";
    QTest::newRow("No error code") << "<smoozik><status>failed</status><error></error></smoozik>" << false << (int) SmoozikManager::ParseError << "Could not parse xml";
    QTest::newRow("Valid error") << "<smoozik><status>failed</status><error><code>1</code><message>Message</message></error></smoozik>" << false << 1 << "Message";
    QTest::newRow("No data") << "<smoozik><status>ok</status></smoozik>" << false << (int) SmoozikManager::ParseError << "Could not parse xml";
    QTest::newRow("Invalid data") << "<smoozik><status>ok</status><data><invalid </data></smoozik>" << false << (int) SmoozikManager::ParseError << "Could not parse xml";
    QTest::newRow("Empty data") << "<smoozik><status>ok</status><data></data></smoozik>" << true << (int) SmoozikManager::NoError << QString();
    QTest::newRow("Valid data") << "<smoozik><status>ok</status><data><xml></xml></data></smoozik>" << true << (int) SmoozikManager::NoError << QString();
}

void TestSmoozikXml::parse()
{
    QFETCH(QString, response);
    QFETCH(bool, parseResult);
    QFETCH(int, errorResult);
    QFETCH(QString, errorMsgResult);

    SmoozikXml *xml = new SmoozikXml(this);
    SimpleHttpServer server(8181);
    server.setResponse("");

    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://127.0.0.1:8181"));
    QNetworkReply *reply;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    server.setResponse(response);
    reply = manager.get(request);
    loop.exec();
    QCOMPARE(xml->parse(reply), parseResult);
    QCOMPARE((int) xml->error(), errorResult);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QCOMPARE(xml->errorMsg().contains(errorMsgResult), true);
#else
    QCOMPARE(xml->errorMsg().contains(errorMsgResult), QBool(true));
#endif
}

void TestSmoozikXml::operators_data()
{
    QTest::addColumn<QString>("response");
    QTest::addColumn<bool>("toMap");
    QTest::addColumn<bool>("toList");
    QTest::addColumn<QString>("toString");

    QTest::newRow("Map response") << "<smoozik><status>ok</status><data><test>hello</test><test2>hello</test2></data></smoozik>" << true << false << QString();
    QTest::newRow("List response") << "<smoozik><status>ok</status><data><test>hello</test><test>hello</test></data></smoozik>" << false << true << QString();
    QTest::newRow("String response") << "<smoozik><status>ok</status><data>hello</data></smoozik>" << false << false << QString("hello");
}

void TestSmoozikXml::operators()
{
    QFETCH(QString, response);
    QFETCH(bool, toMap);
    QFETCH(bool, toList);
    QFETCH(QString, toString);

    SmoozikXml xml;
    SimpleHttpServer server(8181);
    server.setResponse(response);

    QNetworkAccessManager manager;
    QNetworkRequest request(QUrl("http://127.0.0.1:8181"));
    QNetworkReply *reply;
    QEventLoop loop;
    connect(&manager, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));

    reply = manager.get(request);
    loop.exec();

    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["test"] != QVariant(), toMap);
    QCOMPARE(xml[0] != QVariant(), toList);
    QCOMPARE(xml.parsedString(), toString);
}

QTEST_XML_MAIN(TestSmoozikXml)
