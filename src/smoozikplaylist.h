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

#ifndef SMOOZIKPLAYLIST_H
#define SMOOZIKPLAYLIST_H

#include <QObject>
#include <QList>

#include "global.h"
#include "smooziktrack.h"

/**
 * @brief The SmoozikPlaylist class represents a collection of track
 */
class SmoozikPlaylist : public QObject, public QList<SmoozikTrack *> {
    Q_OBJECT
public:
    SMOOZIKLIB_EXPORT explicit SmoozikPlaylist(QObject *parent = 0);

    /**
     * @brief Adds a track to the playlist.
     * @param track
     */
    inline void addTrack(SmoozikTrack *track) {
        append(track);
    }

    /**
     * @brief Adds a track to the playlist.
     * @param localId Local unique Id of the track
     * @param name Name of the track
     * @param artist Artist of the track
     * @param album Album name of the track
     * @param duration Duration of the track
     */
    inline void addTrack(QString localId, QString name, QString artist = QString(), QString album = QString(), uint duration = 0) {
        append(new SmoozikTrack(localId, name, this, artist, album, duration));
    }

    /**
     * @brief Deletes the playlist with all its tracks.
     *
     * Normal deletion would not free memory occupied by tracks.
     */
    SMOOZIKLIB_EXPORT void deleteWithTracks();
};

#endif // SMOOZIKPLAYLIST_H
