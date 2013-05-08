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
#include <QThread>
#include "smoozikmanager.h"
#include "smoozikplaylistfiller.h"
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <phonon/MediaObject>
#include <phonon/AudioOutput>
#include <phonon/MediaSource>
#include <QDesktopServices>
// As slots must be declared no matter the Qt version, QMediaPlayer namespace need to be declared.
namespace QMediaPlayer
{
enum State {
    Empty
};
}
#else
#include <QMediaPlayer>
#include <QMediaPlaylist>
// As slots must be declared no matter the Qt version, Phonon namespace need to be declared.
namespace Phonon
{
class MediaSource;
enum State {
    Empty
};
}
#include <QStandardPaths>
#endif

namespace Ui
{
class SmoozikSimplestClientWindow;
}

class SmoozikSimplestClientWindow : public QMainWindow
{
    Q_ENUMS(State)
    /**
     * @brief This property holds the current state in which the program is.
     * @af state(), setState()
     * @pm _state
     */
    Q_PROPERTY(State state READ state WRITE setState)
    Q_OBJECT

public:
    explicit SmoozikSimplestClientWindow(QWidget *parent = 0);
    ~SmoozikSimplestClientWindow();
    /**
     * @brief The State enum defines states in which the program could be.
     * @sa #state
     */
    enum State {
        Login,
        StartParty,
        SendPlaylist,
        GetTopTracks,
        SendCurrentTrack,
        SendNextTrack
    };
    inline State state() const {
        return _state;
    } /**< @see #state */
    inline void setState(const State &state) {
        _state = state;
    } /**< @see #state */

private:
    Ui::SmoozikSimplestClientWindow *ui;
    SmoozikManager *smoozikManager;
    SmoozikPlaylist *smoozikPlaylist;
    /**
     * @brief Thread that will run the SmoozikPlaylistFiller
     */
    QThread *smoozikPlaylistFillerThread;
    /**
     * @brief Worker class to fill playlist with from folder
     */
    SmoozikPlaylistFiller* smoozikPlaylistFiller;
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
    State _state; /**< @see #state */
    /**
     * @brief Path to the directory currently used to fetch tracks.
     */
    QString _dirName;
    /**
     * @brief Returns index of current track in #smoozikPlaylist
     * @retval 1 if current track cannot be found in #smoozikPlaylist
     */
    int getCurrentTrackIndex();
    /**
     * @brief Returns index of current track in #smoozikPlaylist
     * @retval 1 if current track cannot be found in #smoozikPlaylist
     */
    int getNextTrackIndex();

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
     * @brief Starts a party using #smoozikManager.
     */
    inline void startParty() {
        smoozikManager->startParty();
    }
    /**
     * @brief Get top tracks using #smoozikManager.
     */
    inline void getTopTracks() {
        smoozikManager->getTopTracks();
    }
    /**
     * @brief Starts a party using #smoozikManager.
     */
    inline void sendPlaylist() {
        smoozikManager->sendPlaylist(smoozikPlaylist);
    }
    /**
     * @brief Sets current track in Smoozik server
     */
    void sendCurrentTrack();
    /**
     * @brief Sets next track in Smoozik server
     */
    void sendNextTrack();
    /**
     * @brief Asks user to select a folder containing music files until a non-empty playlist can be filled.
     */
    void retrieveTracksDialog();
    /**
     * @brief Update current track label and next track label with data from playlist
     */
    void updateTrackLabels();
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
    /**
     * @brief Emits signal #playing() or #paused() depending on the new #player state (Qt4).
     */
    void playerStateChanged(const Phonon::State newstate, const Phonon::State);
    /**
     * @brief Emits signal #playing() or #paused() depending on the new #player state (Qt5).
     */
    void playerStateChanged(const QMediaPlayer::State state);
    /**
     * @brief Adds a track to the #smoozikPlaylist.
     */
    inline void addTrackToPlaylist(QString localId, QString name, QString artist = QString(), QString album = QString(), uint duration = 0) {
        smoozikPlaylist->addTrack(localId, name, artist, album, duration);
    }

    /**
     * @brief Displays a message box warning about max playlist size having been reached.
     */
    void maxPlaylistSizeReachedMessage();
    /**
     * @brief Displays a message box warning about no track being present in current directory then call retrieveTracksDialog().
     */
    void noTrackRetrievedMessage();
    /**
     * @brief Plays next track in playlist
     */
    void nextTrack();

signals:
    /**
     * @brief This signal is emitted when user is correctly logged in.
     */
    void loggedIn();
    /**
     * @brief This signal is emitted when party is correctly started.
     */
    void partyStarted();
    /**
     * @brief This signal is emitted when local tracks have been retrieved.
     */
    void tracksRetrieved();
    /**
     * @brief This signal is emitted when playlist has been sent to Smoozik server.
     */
    void playlistSent();
    /**
     * @brief This signal is emitted when current track has been set in local playlist and can be played.
     */
    void currentTrackSet();
    /**
     * @brief This signal is emitted when next track has been set in local playlist and can be played.
     */
    void nextTrackSet();
    /**
     * @brief This signal is emitted when current track has been set in Smoozik server.
     */
    void currentTrackSent();
    /**
     * @brief This signal is emitted when next track has been set in Smoozik server.
     */
    void nextTrackSent();
    /**
     * @brief This signal is emitted when user request disconnection.
     */
    void disconnect();
    /**
     * @brief This signal is emitted when player starts playing.
     */
    void playing();
    /**
     * @brief This signal is emitted when player is paused.
     */
    void paused();
};

#endif // SMOOZIKSIMPLESTCLIENTWINDOW_H
