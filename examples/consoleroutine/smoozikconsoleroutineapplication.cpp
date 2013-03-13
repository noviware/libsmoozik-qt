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

#include "smoozikconsoleroutineapplication.h"

#include <QTextStream>
#include <iostream>
#include "smoozikmanager.h"
#include "smoozikxml.h"
#include "config.h"

SmoozikConsoleRoutineApplication::SmoozikConsoleRoutineApplication(int &argc, char *argv[]) :
QCoreApplication(argc, argv) {
}

int SmoozikConsoleRoutineApplication::run() {
    QTextStream out(stdout);

    out << "\n";
    out << "Starting console example...\n";

    SmoozikManager manager(APIKEY, SECRET);
    SmoozikXml xml;
    QNetworkReply * reply = NULL;

    // Login
    reply = manager.login(USERNAME, PASSWORD);
    if (!xml.parse(reply)) {
        out << "An error occured while logging in.\n";
        out << QString("Error #%1: %2\n").arg(xml.error()).arg(xml.errorMsg());
        exit(-1);
        return -1;
    }

    out << QString("Logged in '%1' venue.\n").arg(xml["place"].toMap()["name"].toString());

    // Start party
    reply = manager.startParty();
    if (!xml.parse(reply)) {
        out << "An error occured while starting party.\n";
        out << QString("Error #%1: %2\n").arg(xml.error()).arg(xml.errorMsg());
        exit(-1);
        return -1;
    }

    out << QString("Party started. Private code is '%1'.\n").arg(xml["party"].toMap()["code"].toString());

    //Create playlist
    SmoozikPlaylist playlist;
    playlist.addTrack("1", "track1", "artist1");
    playlist.addTrack("2", "track2", "artist2");
    playlist.addTrack("3", "track3", "artist3");
    playlist.addTrack("4", "track4", "artist4");
    playlist.addTrack("5", "track5", "artist5");

    out << QString("Playlist created with %1 tracks.\n").arg(playlist.count());

    //Send playlist
    reply = manager.sendPlaylist(&playlist);
    if (!xml.parse(reply)) {
        out << "An error occured while sending playlist.\n";
        out << QString("Error #%1: %2\n").arg(xml.error()).arg(xml.errorMsg());
        exit(-1);
        return -1;
    }

    out << "Playlist sent successfully.\n";

    //Set current track
    srand((uint) QDateTime::currentDateTime().toMSecsSinceEpoch());
    int currentTrackPos = (playlist.count() * (rand() / 1000)) / (RAND_MAX / 1000);
    SmoozikTrack *currentTrack = playlist.value(currentTrackPos);
    reply = manager.setTrack(currentTrack);
    if (!xml.parse(reply)) {
        out << "An error occured while setting current track.\n";
        out << QString("Error #%1: %2\n").arg(xml.error()).arg(xml.errorMsg());
        exit(-1);
        return -1;
    }

    out << QString("Current track set successfully: %1\n").arg(currentTrack->name());

    //Set coming track
    int nextTrackPos = -1;
    while (nextTrackPos < 0 || nextTrackPos == currentTrackPos) {
        nextTrackPos = (playlist.count() * (rand() / 1000)) / (RAND_MAX / 1000);
    }
    SmoozikTrack *nextTrack = playlist.value(nextTrackPos);
    reply = manager.setTrack(nextTrack, 1);
    if (!xml.parse(reply)) {
        out << "An error occured while setting coming track.\n";
        out << QString("Error #%1: %2\n").arg(xml.error()).arg(xml.errorMsg());
        exit(-1);
        return -1;
    }

    out << QString("Coming track set successfully: %1\n").arg(nextTrack->name());

    //Retrieve top tracks
    reply = manager.getTopTracks();
    if (!xml.parse(reply)) {
        out << "An error occured while getting top tracks.\n";
        out << QString("Error #%1: %2\n").arg(xml.error()).arg(xml.errorMsg());
        exit(-1);
        return -1;
    }
    SmoozikPlaylist topTracks(xml["tracks"].toList());

    out << "Top tracks retrieved successfully:\n";
    for (int i = 0; i < topTracks.count(); i++) {
        SmoozikTrack *track = topTracks.value(i);
        out << QString("  - Track #%1: %2 - %3\n").arg(i + 1).arg(track->name()).arg(track->artist());
    }

    out << "Console example finished.\n";

    exit(0);
    return 0;
}
