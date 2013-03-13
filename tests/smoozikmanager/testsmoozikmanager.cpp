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
#include "smoozikmanager.h"
#include "smoozikxml.h"

#include <QDomElement>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QJsonDocument>
#endif

void TestSmoozikManager::constructors() {
    SmoozikManager manager(APIKEY);
    QCOMPARE(manager.apiKey(), QString(APIKEY));
    QCOMPARE(manager.secret(), QString());

    SmoozikManager manager2(APIKEY, SECRET);
    QCOMPARE(manager2.apiKey(), QString(APIKEY));
    QCOMPARE(manager2.secret(), QString(SECRET));
}

void TestSmoozikManager::format() {
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    QNetworkReply *reply = manager.login(MEMBER_USERNAME, MEMBER_PASSWORD);
    QDomDocument xml;
    QCOMPARE(xml.setContent(reply->readAll()), true);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    SmoozikManager manager2(APIKEY, SECRET, SmoozikManager::JSON, true);
    reply = manager2.login(MEMBER_USERNAME, MEMBER_PASSWORD);
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

    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager.login(username, password);
    QCOMPARE(xml.parse(reply), parseResult);
    QCOMPARE((int) xml.error(), errorResult);
    QCOMPARE(xml["sessionKey"].isNull(), sessionKeyNull);
    QCOMPARE(xml["place"].isNull(), placeNull);
}

void TestSmoozikManager::startParty() {
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager.startParty();
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::AccessRestricted);

    reply = manager.login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager.setSessionKey(xml["sessionKey"].toString());
    reply = manager.startParty();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["party"].isNull(), false);
}

void TestSmoozikManager::sendPlaylist() {
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager.login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager.setSessionKey(xml["sessionKey"].toString());
    reply = manager.startParty();

    SmoozikPlaylist playlist;
    playlist.addTrack("1", "track1", "artist1", "album1", 220);
    playlist.addTrack("2", "track2", "artist2");
    playlist.addTrack("3", "track3");
    playlist.addTrack("4", "track4", "artist1", "album1", 220);
    playlist.addTrack("5", "track5", "artist2");

    reply = manager.sendPlaylist(&playlist);
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);
}

void TestSmoozikManager::setTrack() {
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager.login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager.setSessionKey(xml["sessionKey"].toString());
    reply = manager.startParty();

    //Track in playlist
    SmoozikTrack track1("1", "track1", this, "artist1", "album1", 220);
    reply = manager.setTrack(&track1);
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);

    //Track not in playlist
    SmoozikTrack track6("6", "track6", this);
    reply = manager.setTrack(&track6);
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);
}

void TestSmoozikManager::unsetTrack() {
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager.login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager.setSessionKey(xml["sessionKey"].toString());
    manager.startParty();

    SmoozikPlaylist playlist;
    playlist.addTrack("1", "track1", "artist1", "album1", 220);
    playlist.addTrack("2", "track2", "artist2");
    playlist.addTrack("3", "track3");
    playlist.addTrack("4", "track4", "artist1", "album1", 220);
    playlist.addTrack("5", "track5", "artist2");

    manager.sendPlaylist(&playlist);

    SmoozikTrack track1("1", "track1", this, "artist1", "album1", 220);
    manager.setTrack(&track1);
    manager.setTrack("2", QString(), QString(), QString(), 0, 1);
    manager.setTrack("3", QString(), QString(), QString(), 0, 2);
    manager.setTrack("4", QString(), QString(), QString(), 0, 3);
    manager.setTrack("5", QString(), QString(), QString(), 0, 4);

    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].toList().isEmpty(), true);

    reply = manager.unsetTrack(&track1);
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);

    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].toList().count(), 1);
}

void TestSmoozikManager::unsetAllTracks() {
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager.login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager.setSessionKey(xml["sessionKey"].toString());
    manager.startParty();

    SmoozikPlaylist playlist;
    playlist.addTrack("1", "track1", "artist1", "album1", 220);
    playlist.addTrack("2", "track2", "artist2");
    playlist.addTrack("3", "track3");
    playlist.addTrack("4", "track4", "artist1", "album1", 220);
    playlist.addTrack("5", "track5", "artist2");

    manager.sendPlaylist(&playlist);

    manager.setTrack("1", QString(), QString(), QString(), 0, 0);
    manager.setTrack("2", QString(), QString(), QString(), 0, 1);
    manager.setTrack("3", QString(), QString(), QString(), 0, 2);
    manager.setTrack("4", QString(), QString(), QString(), 0, 3);
    manager.setTrack("5", QString(), QString(), QString(), 0, 4);

    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].toList().isEmpty(), true);

    reply = manager.unsetAllTracks();
    QCOMPARE(xml.parse(reply), true);

    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].toList().count(), 5);
}

void TestSmoozikManager::getTopTracks() {
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::AccessRestricted);

    reply = manager.login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager.setSessionKey(xml["sessionKey"].toString());
    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].isNull(), false);
    QCOMPARE(xml.print().isEmpty(), false);

    //Checks limit and offset
    SmoozikXml xml2;
    reply = manager.getTopTracks(3, 2);
    xml2.parse(reply);

    QCOMPARE(xml2["tracks"].toList().count(), 3);
    QCOMPARE(xml2["tracks"].toList()[0].toMap()["track"].toMap()["id"].isNull(), false);
    QCOMPARE(xml2["tracks"].toList()[0].toMap()["track"].toMap()["id"], xml["tracks"].toList()[2].toMap()["track"].toMap()["id"]);

    //Retrieve playlist from xml
    SmoozikPlaylist playlist(xml["tracks"].toList());
    QCOMPARE(playlist.count(), xml["tracks"].toList().count());
    QCOMPARE(playlist.first()->localId(), xml["tracks"].toList()[0].toMap()["track"].toMap()["localId"].toString());
}

QTEST_MAIN(TestSmoozikManager)
