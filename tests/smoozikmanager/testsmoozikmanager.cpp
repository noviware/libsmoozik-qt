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

void TestSmoozikManager::login() {
    SmoozikManager *manager = new SmoozikManager(APIKEY, this, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager->login("error", MANAGER_PASSWORD);
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::AuthenticationFailed);

    reply = manager->login(MANAGER_USERNAME, "error");
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::AuthenticationFailed);

    reply = manager->login(MANAGER_USERNAME, MANAGER_PASSWORD);
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["sessionKey"].isNull(), false);
    QCOMPARE(xml["place"].isNull(), false);
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

    //Checks limit and offset
    SmoozikXml xml2;
    reply = manager->getTopTracks(3, 2);
    xml2.parse(reply);
    QCOMPARE(xml2["tracks"].toMap().count(), 3);
    QCOMPARE(xml2["tracks"].toMap()["0"].toMap()["track"].toMap()["id"].isNull(), false);
    QCOMPARE(xml2["tracks"].toMap()["0"].toMap()["track"].toMap()["id"], xml["tracks"].toMap()["2"].toMap()["track"].toMap()["id"]);
}

QTEST_MAIN(TestSmoozikManager)
