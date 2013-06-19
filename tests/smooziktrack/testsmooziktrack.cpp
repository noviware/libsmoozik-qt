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

#include "testsmooziktrack.h"
#include "smooziktrack.h"
#include "smoozikxml.h"

void TestSmoozikTrack::constructors_data()
{
    QTest::addColumn<QString>("xmlString");
    QTest::addColumn<QString>("localId");
    QTest::addColumn<QString>("name");
    QTest::addColumn<QString>("artist");
    QTest::addColumn<QString>("album");
    QTest::addColumn<uint>("duration");
    QTest::addColumn<QString>("fileName");

    QTest::newRow("Empty xml") << QString() << QString() << QString() << QString() << QString() << (uint)0 << QString();
    QTest::newRow("Minimal required properties") << "<track><localId>id</localId><name>track</name></track>" << "id" << "track" << QString() << QString() << (uint)0  << QString();
    QTest::newRow("No fileName") << "<track><localId>id</localId><name>track</name><album>album</album><artist>artist</artist><duration>220</duration></track>" << "id" << "track"<< "artist" << "album"  << (uint)220 << QString();
    QTest::newRow("All properties") << "<track><localId>id</localId><name>track</name><album>album</album><artist>artist</artist><duration>220</duration><fileName>fileName</fileName></track>" << "id" << "track" << "artist" << "album" << (uint)220 << "fileName";
}

void TestSmoozikTrack::constructors()
{
    QFETCH(QString, xmlString);
    QFETCH(QString, localId);
    QFETCH(QString, name);
    QFETCH(QString, album);
    QFETCH(QString, artist);
    QFETCH(uint, duration);
    QFETCH(QString, fileName);

    QDomDocument doc;
    QVariant variant;
    if(!xmlString.isEmpty()) {
        doc.setContent(xmlString);
        variant = SmoozikXml::parseElement(doc.firstChildElement());
    }

    SmoozikTrack track(doc);
    QCOMPARE(track.localId(), localId);
    QCOMPARE(track.name(), name);
    QCOMPARE(track.album(), album);
    QCOMPARE(track.artist(), artist);
    QCOMPARE(track.duration(), duration);
    QCOMPARE(track.fileName(), fileName);

    SmoozikTrack track2(variant.toMap());
    QCOMPARE(track2.localId(), localId);
    QCOMPARE(track2.name(), name);
    QCOMPARE(track2.album(), album);
    QCOMPARE(track2.artist(), artist);
    QCOMPARE(track2.duration(), duration);
    QCOMPARE(track2.fileName(), fileName);

    SmoozikTrack track3(localId, name, 0, artist, album, duration, fileName);
    QCOMPARE(track3.localId(), localId);
    QCOMPARE(track3.name(), name);
    QCOMPARE(track3.album(), album);
    QCOMPARE(track3.artist(), artist);
    QCOMPARE(track3.duration(), duration);
    QCOMPARE(track3.fileName(), fileName);
}

QTEST_XML_MAIN(TestSmoozikTrack)
