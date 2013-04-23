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

#ifndef SMOOZIKSIMPLESTCLIENTWINDOW_H
#define SMOOZIKSIMPLESTCLIENTWINDOW_H

#include <QMainWindow>
#include <QDir>
#include "smoozikmanager.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/MediaSource>
#include <QDesktopServices>
#else
#include <QMediaPlayer>
#include <QMediaPlaylist>
// As updatePlaylistCurrentIndex() slot must be declared no matter the Qt version, Phonon::MediaSource class need to be declared.
namespace Phonon
{
class MediaSource;
}
#include <QStandardPaths>
#endif

namespace Ui
{
class SmoozikSimplestClientWindow;
}

class SmoozikSimplestClientWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit SmoozikSimplestClientWindow(QWidget *parent = 0);
    ~SmoozikSimplestClientWindow();

private:
    Ui::SmoozikSimplestClientWindow *ui;
    SmoozikManager *smoozikManager;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    /**
     * @brief Player used to play music files (Qt4).
     */
    Phonon::MediaObject *player;
    /**
     * @brief Playlist containing music files to play (Qt4).
     */
    QList<Phonon::MediaSource> playlist;
    /**
     * @brief Index of the current track in #playlist (Qt4).
     */
    int playlistCurrentIndex;
#else
    /**
     * @brief Player used to play music files (Qt5).
     */
    QMediaPlayer *player;
    /**
     * @brief Playlist containing music files to play (Qt5).
     */
    QMediaPlaylist playlist;
#endif
    /**
     * @brief Adds tracks from @i directory to @i playlist.
     * @retval 0 All tracks from folder were added to the playlist.
     * @retval -1 All tracks were not added (either because an error occured or because playlist max size has been reached).
     */
    int addTracksToPlaylist(const QDir *directory, SmoozikPlaylist *playlist);

private slots:
    /**
     * @brief Retrieves reply from network request and processes them.
     * @param reply QNetworkReply to process
     */
    void processNetworkReply(QNetworkReply *reply);
    /**
     * @brief Retrieves username and password and uses them to log user in.
     */
    void submitLogin();
    /**
     * @brief Displays error message and requests new login.
     */
    void loginError(QString errorMsg);
    /**
     * @brief Asks user to select a folder containing music files until a non-empty playlist can be filled.
     */
    void retrieveTracksDialog();
    /**
     * @brief Update playlistCurrentIndex to match newSource (Qt4).
     */
    inline void updatePlaylistCurrentIndex(const Phonon::MediaSource &newSource) {
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
        playlistCurrentIndex = playlist.indexOf(newSource);
#else
        (void)newSource;
#endif
    }


signals:
    /**
     * @brief This signal is emitted when user is correctly logged in and party is correctly started.
     */
    void ready();
    /**
     * @brief This signal is emitted when user request disconnection.
     */
    void disconnect();
};

#endif // SMOOZIKSIMPLESTCLIENTWINDOW_H
