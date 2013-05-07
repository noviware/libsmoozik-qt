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

#ifndef SMOOZIKPLAYLISTFILLER_H
#define SMOOZIKPLAYLISTFILLER_H

#include <QDir>
#include <QMutex>
#include "smoozikplaylist.h"
#include "fileref.h"
#include "tag.h"

/**
 * @brief The SmoozikPlaylistFiller class provides function to add tracks from a directory to a SmoozikPlaylist.
 *
 * It is used to fill SmoozikPlaylist from another thread.
 */
class SmoozikPlaylistFiller : public QObject
{
    Q_OBJECT
public:
    explicit SmoozikPlaylistFiller(SmoozikPlaylist *smoozikPlaylist, QObject *parent = 0);
    /**
     * @brief Sets path of directory in which SmoozikPlaylistFiller will look for track to add to #_smoozikPlaylist.
     */
    inline void setDirName(const QString &dirName) {
        _directory.setPath(dirName);
    }

private:
    QDir _directory;
    SmoozikPlaylist *_smoozikPlaylist;
    QMutex mutex;
    /**
     * @brief Indicates that the filling must be aborted.
     */
    bool _abort;

    /**
     * @brief Adds tracks from @em directory to @em playlist.
     * @return Number of track added
     * @retval -1 If process were aborted
     */
    int addTracksToPlaylist(const QDir *directory);

signals:
    /**
     * @brief This signal is emitted when a track has been retrieved.
     */
    void trackFound(QString localId, QString name, QString artist = QString(), QString album = QString(), uint duration = 0);
    /**
     * @brief This signal is emitted when local tracks have been retrieved.
     */
    void tracksRetrieved();
    /**
     * @brief This signal is emitted when no local track could been retrieved from current folder.
     */
    void noTrackRetrieved();
    /**
     * @brief This signal is emitted when max playlist size has been reached. No further track will be added to the playlist.
     */
    void maxPlaylistSizeReached();
    /**
     * @brief This signal is emitted when fillPlaylist() function has finished.
     */
    void finished();

public slots:
    /**
     * @brief Adds tracks from @em directory to @em playlist.
     *
     * Emits signal tracksRetrieved() if tracks were retrieved.
     * Emits signal noTrackRetrieved() if no track could be retrieved.
     * Emits signal maxPlaylistSizeReached() if max number of tracks in playlist has been reached (this signal is sent before tracksRetrieved()).
     */
    void fillPlaylist();
    /**
     * @brief Requests the filling to abort.
     *
     * Sets #_abort to true.
     */
    void abort();
};

#endif // SMOOZIKPLAYLISTFILLER_H
