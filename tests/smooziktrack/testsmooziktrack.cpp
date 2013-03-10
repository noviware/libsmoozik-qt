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

void TestSmoozikTrack::constructors() {
    QString str = "<track><localId>1</localId><name>track1</name></track>";
    QDomDocument doc;
    doc.setContent(str);

    SmoozikTrack track(doc);
    QCOMPARE(track.localId(), QString("1"));
    QCOMPARE(track.name(), QString("track1"));
    QCOMPARE(track.album().isEmpty(), true);
    QCOMPARE(track.artist().isEmpty(), true);
    QCOMPARE(track.duration(), (uint) 0);

    QString str2 = "<track><localId>2</localId><name>track2</name><album>album2</album><artist>artist2</artist><duration>220</duration></track>";
    QDomDocument doc2;
    doc2.setContent(str2);
    QVariant variant = SmoozikXml::parseElement(doc2.firstChildElement());

    SmoozikTrack track2(variant.toMap());
    QCOMPARE(track2.localId(), QString("2"));
    QCOMPARE(track2.name(), QString("track2"));
    QCOMPARE(track2.album(), QString("album2"));
    QCOMPARE(track2.artist(), QString("artist2"));
    QCOMPARE(track2.duration(), (uint) 220);

    QDomDocument emptyDoc;
    SmoozikTrack track3(emptyDoc);
    QCOMPARE(track3.localId().isEmpty(), true);
    QCOMPARE(track3.name().isEmpty(), true);
    QCOMPARE(track3.album().isEmpty(), true);
    QCOMPARE(track3.artist().isEmpty(), true);
    QCOMPARE(track3.duration(), (uint) 0);

    QVariantMap emptyMap;
    SmoozikTrack track4(emptyMap);
    QCOMPARE(track4.localId().isEmpty(), true);
    QCOMPARE(track4.name().isEmpty(), true);
    QCOMPARE(track4.album().isEmpty(), true);
    QCOMPARE(track4.artist().isEmpty(), true);
    QCOMPARE(track4.duration(), (uint) 0);
}

QTEST_MAIN(TestSmoozikTrack)
