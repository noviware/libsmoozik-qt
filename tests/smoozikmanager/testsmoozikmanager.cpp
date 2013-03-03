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

#include "testsmoozikmanager.h"

#include <QDomElement>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QJsonDocument>
#endif

void TestSmoozikManager::format() {
    SmoozikManager *manager = new SmoozikManager(APIKEY, this, SECRET, SmoozikManager::XML, true);
    QNetworkReply *reply = manager->login(MEMBER_USERNAME, MEMBER_PASSWORD);
    QDomDocument xml;
    QCOMPARE(xml.setContent(reply->readAll()), true);
    manager->deleteLater();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    manager = new SmoozikManager(APIKEY, this, SECRET, SmoozikManager::JSON, true);
    reply = manager->login(MEMBER_USERNAME, MEMBER_PASSWORD);
    QByteArray jsonData = reply->readAll();
    QJsonDocument json;
    QCOMPARE(json.fromJson(jsonData).isEmpty(), false);
#endif
}

void TestSmoozikManager::login_data() {
    QTest::addColumn<QString>("username");
    QTest::addColumn<QString>("password");
    QTest::addColumn<bool>("parseResult");
    QTest::addColumn<int>("errorResult");
    QTest::addColumn<bool>("sessionKeyNull");
    QTest::addColumn<bool>("placeNull");

    QTest::newRow("Wrong username") << "error" << MANAGER_PASSWORD << false << (int) SmoozikManager::AuthenticationFailed << true << true;
    QTest::newRow("Wrong password") << MANAGER_USERNAME << "error" << false << (int) SmoozikManager::AuthenticationFailed << true << true;
    QTest::newRow("Wrong username and password") << "error" << "error" << false << (int) SmoozikManager::AuthenticationFailed << true << true;
    QTest::newRow("Success (member)") << MEMBER_USERNAME << MEMBER_PASSWORD << true << (int) SmoozikManager::NoError << false << true;
    QTest::newRow("Success (manager)") << MANAGER_USERNAME << MANAGER_PASSWORD << true << (int) SmoozikManager::NoError << false << false;
}

void TestSmoozikManager::login() {
    QFETCH(QString, username);
    QFETCH(QString, password);
    QFETCH(bool, parseResult);
    QFETCH(int, errorResult);
    QFETCH(bool, sessionKeyNull);
    QFETCH(bool, placeNull);

    SmoozikManager *manager = new SmoozikManager(APIKEY, this, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager->login(username, password);
    QCOMPARE(xml.parse(reply), parseResult);
    QCOMPARE((int) xml.error(), errorResult);
    QCOMPARE(xml["sessionKey"].isNull(), sessionKeyNull);
    QCOMPARE(xml["place"].isNull(), placeNull);
}

void TestSmoozikManager::startParty() {
    SmoozikManager *manager = new SmoozikManager(APIKEY, this, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager->startParty();
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::AccessRestricted);

    reply = manager->login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager->setSessionKey(xml["sessionKey"].toString());
    reply = manager->startParty();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["party"].isNull(), false);
}

void TestSmoozikManager::getTopTracks() {
    SmoozikManager *manager = new SmoozikManager(APIKEY, this, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager->getTopTracks();
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::AccessRestricted);

    reply = manager->login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager->setSessionKey(xml["sessionKey"].toString());
    reply = manager->getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].isNull(), false);
    QCOMPARE(xml.print().isEmpty(), false);

    //Checks limit and offset
    SmoozikXml xml2;
    reply = manager->getTopTracks(3, 2);
    xml2.parse(reply);
    QCOMPARE(xml2["tracks"].toMap().count(), 3);
    QCOMPARE(xml2["tracks"].toMap()["0"].toMap()["track"].toMap()["id"].isNull(), false);
    QCOMPARE(xml2["tracks"].toMap()["0"].toMap()["track"].toMap()["id"], xml["tracks"].toMap()["2"].toMap()["track"].toMap()["id"]);
}

QTEST_MAIN(TestSmoozikManager)
