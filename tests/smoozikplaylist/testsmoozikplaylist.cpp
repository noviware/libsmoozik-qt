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

#include "testsmoozikplaylist.h"
#include "smoozikplaylist.h"
#include "smoozikxml.h"

void TestSmoozikPlaylist::constructors()
{
    SmoozikPlaylist playlist;
    QCOMPARE(playlist.isEmpty(), true);

    QString str = "<tracks><track><localId>1</localId><name>track1</name></track><track><localId>2</localId><name>track2</name></track></tracks>";
    QDomDocument doc;
    doc.setContent(str);

    SmoozikPlaylist playlist2(doc);
    QCOMPARE(playlist2.isEmpty(), false);
    QCOMPARE(playlist2.first()->localId(), QString("1"));
    QCOMPARE(playlist2.first()->name(), QString("track1"));
    QCOMPARE(playlist2.last()->localId(), QString("2"));
    QCOMPARE(playlist2.last()->name(), QString("track2"));

    QVariant variant = SmoozikXml::parseElement(doc.firstChildElement());

    SmoozikPlaylist playlist3(variant.toList());
    QCOMPARE(playlist3.isEmpty(), false);
    QCOMPARE(playlist3.first()->localId(), QString("1"));
    QCOMPARE(playlist3.first()->name(), QString("track1"));
    QCOMPARE(playlist3.last()->localId(), QString("2"));
    QCOMPARE(playlist3.last()->name(), QString("track2"));

    QDomDocument emptyDoc;
    SmoozikPlaylist playlist4(emptyDoc);
    QCOMPARE(playlist4.isEmpty(), true);

    QVariantList emptyList;
    SmoozikPlaylist playlist5(emptyList);
    QCOMPARE(playlist5.isEmpty(), true);
}

void TestSmoozikPlaylist::addTrack()
{
    SmoozikPlaylist playlist;
    playlist.addTrack("1", "track1", "artist1", "album1", 220);
    QCOMPARE(playlist.first()->localId(), QString("1"));
    QCOMPARE(playlist.first()->name(), QString("track1"));
    QCOMPARE(playlist.first()->artist(), QString("artist1"));
    QCOMPARE(playlist.first()->album(), QString("album1"));
    QCOMPARE(playlist.first()->duration(), (uint) 220);

    SmoozikTrack track("2", "track2", &playlist, "artist2");
    playlist.addTrack(&track);
    QCOMPARE(playlist.last()->localId(), track.localId());
    QCOMPARE(playlist.last()->name(), track.name());
    QCOMPARE(playlist.last()->artist(), track.artist());
    QCOMPARE(playlist.last()->album(), track.album());
    QCOMPARE(playlist.last()->duration(), track.duration());

    QCOMPARE(playlist.count(), 2);
    playlist.addTrack("1", "track1");
    QCOMPARE(playlist.count(), 2);
}

void TestSmoozikPlaylist::addTracks()
{
    SmoozikPlaylist playlist;
    QCOMPARE(playlist.isEmpty(), true);
    QCOMPARE(playlist.count(), 0);

    QString str = "<tracks><track><localId>1</localId><name>track1</name></track><track><localId>2</localId><name>track2</name></track></tracks>";
    QDomDocument doc;
    doc.setContent(str);

    playlist.addTracks(doc);
    QCOMPARE(playlist.isEmpty(), false);
    QCOMPARE(playlist.first()->localId(), QString("1"));
    QCOMPARE(playlist.first()->name(), QString("track1"));
    QCOMPARE(playlist.last()->localId(), QString("2"));
    QCOMPARE(playlist.last()->name(), QString("track2"));
    QCOMPARE(playlist.count(), 2);

    QString str2 = "<tracks><track><localId>1</localId><name>track1</name></track><track><localId>3</localId><name>track3</name></track></tracks>";
    QDomDocument doc2;
    doc2.setContent(str2);
    QVariant variant = SmoozikXml::parseElement(doc2.firstChildElement());

    playlist.addTracks(variant.toList());
    QCOMPARE(playlist.isEmpty(), false);
    QCOMPARE(playlist.first()->localId(), QString("1"));
    QCOMPARE(playlist.first()->name(), QString("track1"));
    QCOMPARE(playlist.last()->localId(), QString("3"));
    QCOMPARE(playlist.last()->name(), QString("track3"));
    QCOMPARE(playlist.count(), 3);
}

void TestSmoozikPlaylist::qListAggregation()
{
    SmoozikPlaylist playlist;
    QCOMPARE(playlist.isEmpty(), true);

    playlist.addTrack("1", "track1", "artist1", "album1", 220);
    SmoozikTrack *track1 = playlist.first();
    SmoozikTrack *track2 = new SmoozikTrack("2", "track2", &playlist, "artist2");
    playlist.addTrack(track2);
    SmoozikTrack *track3 = new SmoozikTrack("3", "track3", &playlist, "artist3");
    playlist.addTrack(track3);
    playlist.addTrack("4", "track4");
    playlist.addTrack("5", "track5");
    playlist.addTrack("6", "track6");
    playlist.addTrack("7", "track7");
    playlist.addTrack("8", "track8");
    playlist.addTrack("9", "track9");

    QCOMPARE(playlist.contains("2"), true);
    QCOMPARE(playlist.contains("A"), false);
    QCOMPARE(playlist.indexOf("2"), 1);
    QCOMPARE(playlist.count(), 9);
    QCOMPARE(playlist.size(), 9);
    QCOMPARE(playlist.first()->localId(), track1->localId());
    QCOMPARE(playlist.isEmpty(), false);
    QCOMPARE(playlist.last()->localId(), QString("9"));
    playlist.removeFirst(); //list: 2, 3, 4, 5, 6, 7, 8, 9
    QCOMPARE(playlist.first()->localId(), QString("2"));
    playlist.removeLast(); //list: 2, 3, 4, 5, 6, 7, 8
    QCOMPARE(playlist.last()->localId(), QString("8"));
    playlist.removeAt(1); //list: 2, 4, 5, 6, 7, 8
    QCOMPARE(playlist.contains("3"), false);
    QCOMPARE(playlist.takeFirst()->localId(), QString("2")); //list: 4, 5, 6, 7, 8
    QCOMPARE(playlist.contains("2"), false);
    QCOMPARE(playlist.takeLast()->localId(), QString("8")); //list: 4, 5, 6, 7
    QCOMPARE(playlist.contains("8"), false);
    QCOMPARE(playlist.takeAt(1)->localId(), QString("5")); //list: 4, 6, 7, 8
    QCOMPARE(playlist.contains("5"), false);
    QCOMPARE(playlist.value(1)->localId(), QString("6"));
    playlist.clear();
    QCOMPARE(playlist.isEmpty(), true);
}

void TestSmoozikPlaylist::deleteTracks()
{
    SmoozikPlaylist *playlist = new SmoozikPlaylist(this);
    playlist->addTrack("1", "track1", "artist1", "album1", 220);
    SmoozikTrack *track1 = playlist->first();
    SmoozikTrack *track2 = new SmoozikTrack("2", "track2", playlist, "artist2");
    playlist->addTrack(track2);
    SmoozikTrack *track3 = new SmoozikTrack("3", "track3", this, "artist3");
    playlist->addTrack(track3);

    QSignalSpy spy1(track1, SIGNAL(destroyed()));
    QSignalSpy spy2(track2, SIGNAL(destroyed()));
    QSignalSpy spy3(track3, SIGNAL(destroyed()));

    playlist->deleteTracks();

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    QCOMPARE(spy3.count(), 1);
}

void TestSmoozikPlaylist::childrenDeletion()
{
    SmoozikPlaylist *playlist = new SmoozikPlaylist(this);
    playlist->addTrack("1", "track1", "artist1", "album1", 220);
    SmoozikTrack *track1 = playlist->first();
    SmoozikTrack *track2 = new SmoozikTrack("2", "track2", playlist, "artist2");
    playlist->addTrack(track2);
    SmoozikTrack *track3 = new SmoozikTrack("3", "track3", this, "artist3");
    playlist->addTrack(track3);

    QSignalSpy spy1(track1, SIGNAL(destroyed()));
    QSignalSpy spy2(track2, SIGNAL(destroyed()));
    QSignalSpy spy3(track3, SIGNAL(destroyed()));

    delete playlist;

    QCOMPARE(spy1.count(), 1);
    QCOMPARE(spy2.count(), 1);
    // Track 3 should not be deleted as it has been outside playlist and has different parent
    QCOMPARE(spy3.count(), 0);
}

QTEST_XML_MAIN(TestSmoozikPlaylist)
