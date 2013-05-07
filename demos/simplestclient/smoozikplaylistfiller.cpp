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

#include "smoozikplaylistfiller.h"

SmoozikPlaylistFiller::SmoozikPlaylistFiller(SmoozikPlaylist *smoozikPlaylist, QObject *parent) :
    QObject(parent)
{
    _smoozikPlaylist = smoozikPlaylist;
    _abort = false;
}

int SmoozikPlaylistFiller::addTracksToPlaylist(const QDir *directory)
{
    mutex.lock();
    _abort = false;
    mutex.unlock();

    int res = 0;

    foreach(QString fileName, directory->entryList()) {

        mutex.lock();
        bool abort = _abort;
        mutex.unlock();
        if (abort) {
            res = -1;
            break;
        }

        if (fileName != ".." && fileName != ".") {
            QString fullPathFileName = directory->filePath(fileName);

            QFileInfo fileInfo(fullPathFileName);

            // If it is a directory, use addTracksToPlaylist recursively.
            if (fileInfo.isDir()) {
                QDir subdirectory(fullPathFileName);
                int req = addTracksToPlaylist(&subdirectory);
                if (req == -1) {
                    break;
                }
                res += req;
            }

            // Else try to add track to playlist.
            else {

                TagLib::FileRef mediaFileRef(QFile::encodeName(fullPathFileName).constData());
                if (!mediaFileRef.isNull()) {

                    QString name, artist, album;
                    name = TStringToQString(mediaFileRef.tag()->title());
                    artist = TStringToQString(mediaFileRef.tag()->artist());
                    album = TStringToQString(mediaFileRef.tag()->album());

                    if (name != QString()) {
                        emit trackFound(fullPathFileName, name, artist, album);

                        if (res >= MAX_ADVISED_PLAYLIST_SIZE) {

                            emit maxPlaylistSizeReached();
                            break;
                        }

                        res ++;
                    }
                }
            }
        }
    }

    return res;
}

void SmoozikPlaylistFiller::fillPlaylist()
{
    if (addTracksToPlaylist(&_directory) > 0) {
        emit tracksRetrieved();
    } else {
        emit noTrackRetrieved();
    }
    emit finished();
}

void SmoozikPlaylistFiller::abort() {
    mutex.lock();
    _abort = true;
    mutex.unlock();
}
