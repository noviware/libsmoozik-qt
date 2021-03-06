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

QNetworkReply *TestSmoozikManager::startParty(SmoozikManager *manager)
{
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager->login(MANAGER_USERNAME, MANAGER_PASSWORD);
    xml.parse(reply);
    manager->setSessionKey(xml["sessionKey"].toString());
    return manager->startParty();
}

QNetworkReply *TestSmoozikManager::joinParty(SmoozikManager *manager, const QString &partyId)
{
    SmoozikXml xml;
    QNetworkReply *reply;

    reply = manager->login(MEMBER_USERNAME, MEMBER_PASSWORD);
    xml.parse(reply);
    manager->setSessionKey(xml["sessionKey"].toString());
    return manager->joinParty(partyId);
}

void TestSmoozikManager::set5Tracks(SmoozikManager *manager)
{
    SmoozikPlaylist playlist;
    playlist.addTrack("1", "track1", "artist1", "album1", 220);
    playlist.addTrack("2", "track2", "artist2");
    playlist.addTrack("3", "track3");
    playlist.addTrack("4", "track4", "artist1", "album1", 220);
    playlist.addTrack("5", "track5", "artist2");

    manager->sendPlaylist(&playlist);

    manager->setTrack("1", QString(), QString(), QString(), 0, 0);
    manager->setTrack("2", QString(), QString(), QString(), 0, 1);
    manager->setTrack("3", QString(), QString(), QString(), 0, 2);
    manager->setTrack("4", QString(), QString(), QString(), 0, 3);
    manager->setTrack("5", QString(), QString(), QString(), 0, 4);

    QNetworkReply *reply = manager->getTopTracks();
    SmoozikXml xml;
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].toList().isEmpty(), false);
    QCOMPARE(xml["tracks"].toList().count(), 4);
}

void TestSmoozikManager::constructors()
{
    SmoozikManager manager(APIKEY);
    QCOMPARE(manager.apiKey(), QString(APIKEY));
    QCOMPARE(manager.secret(), QString());

    SmoozikManager manager2(APIKEY, SECRET);
    QCOMPARE(manager2.apiKey(), QString(APIKEY));
    QCOMPARE(manager2.secret(), QString(SECRET));
}

void TestSmoozikManager::format()
{
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

void TestSmoozikManager::login_data()
{
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

void TestSmoozikManager::login()
{
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

void TestSmoozikManager::startParty()
{
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

void TestSmoozikManager::joinParty()
{
    QNetworkReply *reply;
    SmoozikXml xml;
    SmoozikManager managerManager(APIKEY, SECRET, SmoozikManager::XML, true);
    reply = startParty(&managerManager);
    xml.parse(reply);
    QString partyId = xml["party"].toMap()["id"].toString();
    reply = managerManager.forceDisconnectUsers(1);

    SmoozikManager memberManager(APIKEY, SECRET, SmoozikManager::XML, true);

    reply = memberManager.joinParty(partyId);
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::AccessRestricted);

    reply = memberManager.login(MEMBER_USERNAME, MEMBER_PASSWORD);
    xml.parse(reply);
    memberManager.setSessionKey(xml["sessionKey"].toString());

    reply = memberManager.joinParty("error");
    QCOMPARE(xml.parse(reply), false);
    QCOMPARE(xml.error(), SmoozikManager::InvalidPartyId);

    reply = memberManager.joinParty(partyId);
    QCOMPARE(xml.parse(reply), true);
}

void TestSmoozikManager::sendPlaylist()
{
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    startParty(&manager);

    SmoozikPlaylist playlist;
    playlist.addTrack("1", "track1", "artist1", "album1", 220);
    playlist.addTrack("2", "track2+&= \"~-/\\:.//%2B%25%41", "artist2+&= \"~-/\\:.//%2B");
    playlist.addTrack("3", "track3");
    playlist.addTrack("4", "track4", "artist1", "album1", 220);
    playlist.addTrack("5", "track5", "artist2+&= \"~-/\\:.//%2B");

    QNetworkReply *reply = manager.sendPlaylist(&playlist);
    SmoozikXml xml;
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);
}

void TestSmoozikManager::setTrack()
{
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    startParty(&manager);

    //Track in playlist
    SmoozikTrack track1("1", "track1", this, "artist1", "album1", 220);
    QNetworkReply *reply = manager.setTrack(&track1);
    SmoozikXml xml;
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);

    //Track not in playlist
    SmoozikTrack track6("6", "track6", this);
    reply = manager.setTrack(&track6);
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);
}

void TestSmoozikManager::unsetTrack()
{
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    startParty(&manager);
    set5Tracks(&manager);

    SmoozikTrack track1("1", "track1", this, "artist1", "album1", 220);
    QNetworkReply *reply = manager.unsetTrack(&track1);
    SmoozikXml xml;
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml.error(), SmoozikManager::NoError);

    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].toList().count(), 5);
}

void TestSmoozikManager::unsetAllTracks()
{
    SmoozikManager manager(APIKEY, SECRET, SmoozikManager::XML, true);
    startParty(&manager);
    set5Tracks(&manager);

    QNetworkReply *reply = manager.unsetAllTracks();
    SmoozikXml xml;
    QCOMPARE(xml.parse(reply), true);

    reply = manager.getTopTracks();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["tracks"].toList().count(), 5);
}

void TestSmoozikManager::getTopTracks()
{
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

void TestSmoozikManager::forceDisconnectUsers()
{
    QNetworkReply *reply;
    SmoozikXml xml;
    SmoozikManager managerManager(APIKEY, SECRET, SmoozikManager::XML, true);
    reply = startParty(&managerManager);
    xml.parse(reply);
    QString partyId = xml["party"].toMap()["id"].toString();
    reply = managerManager.forceDisconnectUsers(1);

    SmoozikManager memberManager(APIKEY, SECRET, SmoozikManager::XML, true);
    joinParty(&memberManager, partyId);

    QEventLoop loop;
    QTimer::singleShot(10000, &loop, SLOT(quit()));
    loop.exec();

    reply = managerManager.forceDisconnectUsers();
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["disconnectedUserCount"].toString(), QString::number(0));

    reply = managerManager.forceDisconnectUsers(1);
    QCOMPARE(xml.parse(reply), true);
    QCOMPARE(xml["disconnectedUserCount"].toString(), QString::number(1));
}

QTEST_XML_MAIN(TestSmoozikManager)
