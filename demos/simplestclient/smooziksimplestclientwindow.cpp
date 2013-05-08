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

#include "smooziksimplestclientwindow.h"
#include "ui_smooziksimplestclientwindow.h"
#include "config.h"
#include "smoozikxml.h"

#include <QStateMachine>
#include <QFileDialog>
#include <QMessageBox>

SmoozikSimplestClientWindow::SmoozikSimplestClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SmoozikSimplestClientWindow)
{
    ui->setupUi(this);

    // Initialize SmoozikManager
    smoozikManager = new SmoozikManager(APIKEY, SECRET, SmoozikManager::XML, false, this);
    smoozikPlaylist = new SmoozikPlaylist;
    connect(smoozikManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processNetworkReply(QNetworkReply*)));

    // Initialize music directory
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    _dirName = QDesktopServices::storageLocation(QDesktopServices::MusicLocation);
#else
    _dirName = QStandardPaths::writableLocation(QStandardPaths::MusicLocation);
#endif

    // Initialize playlist filler
    ui->setupUi(this);
    smoozikPlaylistFillerThread = new QThread();
    smoozikPlaylistFiller = new SmoozikPlaylistFiller(smoozikPlaylist);
    smoozikPlaylistFiller->moveToThread(smoozikPlaylistFillerThread);
    connect(smoozikPlaylistFiller, SIGNAL(trackFound(QString,QString,QString,QString,uint)), this, SLOT(addTrackToPlaylist(QString,QString,QString,QString,uint)));
    connect(smoozikPlaylistFiller, SIGNAL(tracksRetrieved()), this, SIGNAL(tracksRetrieved()));
    connect(smoozikPlaylistFiller, SIGNAL(noTrackRetrieved()), this, SLOT(noTrackRetrievedMessage()));
    connect(smoozikPlaylistFiller, SIGNAL(maxPlaylistSizeReached()), this, SLOT(maxPlaylistSizeReachedMessage()));
    connect(smoozikPlaylistFillerThread, SIGNAL(started()), smoozikPlaylistFiller, SLOT(fillPlaylist()));
    connect(smoozikPlaylistFiller, SIGNAL(finished()), smoozikPlaylistFillerThread, SLOT(quit()), Qt::DirectConnection);

    // Initialize player
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    player = new Phonon::MediaObject(this);
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(player, audioOutput);
    playlistCurrentIndex = 0;
    connect(player, SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SLOT(updatePlaylistCurrentIndex(Phonon::MediaSource)));
    connect(player, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(playerStateChanged(Phonon::State,Phonon::State)));
#else
    player = new QMediaPlayer(this);
    playlist.setPlaybackMode(QMediaPlaylist::Sequential);
    player->setPlaylist(&playlist);
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStateChanged(QMediaPlayer::State)));
#endif
    connect(ui->playButton, SIGNAL(clicked()), player, SLOT(play()));
    connect(ui->pauseButton, SIGNAL(clicked()), player, SLOT(pause()));

    // Initialize main state machine which controls what is displayed
    QStateMachine *mainStateMachine = new QStateMachine(this);
    QState *mainState = new QState(mainStateMachine);
    QState *loginState = new QState(mainState);
    QState *startPartyState = new QState(mainState);
    QState *retrieveTracksState = new QState(mainState);
    QState *sendPlaylistState = new QState(mainState);
    QState *getTopTracksState = new QState(mainState);
    QState *partyState = new QState(mainState);
    QState *waitingState = new QState(partyState);
    QState *sendCurrentTrackState = new QState(partyState);
    QState *sendNextTrackState = new QState(partyState);

    QStateMachine *playerStateMachine = new QStateMachine(this);
    QState *playerState = new QState(playerStateMachine);
    QState *playingState = new QState(playerState);
    QState *pausedState = new QState(playerState);

    // Define state initial states and transitions
    mainStateMachine->setInitialState(mainState);
    mainState->setInitialState(loginState);
    partyState->setInitialState(waitingState);
    playerStateMachine->setInitialState(playerState);
    playerState->setInitialState(pausedState);

    mainState->addTransition(this, SIGNAL(disconnect()), loginState);
    loginState->addTransition(this, SIGNAL(loggedIn()), startPartyState);
    startPartyState->addTransition(this, SIGNAL(partyStarted()), retrieveTracksState);
    retrieveTracksState->addTransition(this, SIGNAL(tracksRetrieved()), sendPlaylistState);
    sendPlaylistState->addTransition(this, SIGNAL(playlistSent()), getTopTracksState);
    getTopTracksState->addTransition(this, SIGNAL(currentTrackSet()), sendCurrentTrackState);
    sendCurrentTrackState->addTransition(this, SIGNAL(currentTrackSent()), getTopTracksState);
    getTopTracksState->addTransition(this, SIGNAL(nextTrackSet()), sendNextTrackState);
    sendNextTrackState->addTransition(this, SIGNAL(nextTrackSent()), waitingState);
    waitingState->addTransition(player, SIGNAL(currentMediaChanged(QMediaContent)), sendCurrentTrackState);

    playerState->addTransition(this, SIGNAL(playing()), playingState);
    playerState->addTransition(this, SIGNAL(paused()), pausedState);

    // Define state properties
    loginState->assignProperty(this, "state", Login);
    loginState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->loginPage));
    loginState->assignProperty(ui->loginButton, "enabled", true);
    loginState->assignProperty(ui->usernameLineEdit, "enabled", true);
    loginState->assignProperty(ui->passwordLineEdit, "enabled", true);
    loginState->assignProperty(ui->loginStateLabel, "text", QString());

    startPartyState->assignProperty(this, "state", StartParty);
    startPartyState->assignProperty(ui->loginStateLabel, "text", tr("Starting party..."));

    retrieveTracksState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->loadingPage));
    retrieveTracksState->assignProperty(ui->loginStateLabel, "text", tr("Connected"));
    retrieveTracksState->assignProperty(ui->loadingLabel, "text", tr("Retrieving tracks..."));

    sendPlaylistState->assignProperty(this, "state", SendPlaylist);
    sendPlaylistState->assignProperty(ui->loadingLabel, "text", tr("Sending playlist..."));

    getTopTracksState->assignProperty(this, "state", GetTopTracks);
    getTopTracksState->assignProperty(ui->loadingLabel, "text", tr("Get top tracks..."));
    getTopTracksState->assignProperty(ui->nextButton, "enabled", false);

    partyState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->playerPage));

    sendCurrentTrackState->assignProperty(this, "state", SendCurrentTrack);
    sendCurrentTrackState->assignProperty(ui->nextButton, "enabled", false);

    sendNextTrackState->assignProperty(this, "state", SendNextTrack);
    sendNextTrackState->assignProperty(ui->nextButton, "enabled", false);

    waitingState->assignProperty(ui->nextButton, "enabled", true);

    playingState->assignProperty(ui->playButton, "visible", false);
    playingState->assignProperty(ui->pauseButton, "visible", true);

    pausedState->assignProperty(ui->playButton, "visible", true);
    pausedState->assignProperty(ui->pauseButton, "visible", false);

    // Connect states and actions
    connect(startPartyState, SIGNAL(entered()), this, SLOT(startParty()));
    connect(retrieveTracksState, SIGNAL(entered()), this, SLOT(retrieveTracksDialog()));
    connect(sendPlaylistState, SIGNAL(entered()), this, SLOT(sendPlaylist()));
    connect(getTopTracksState, SIGNAL(entered()), this, SLOT(getTopTracks()));
    connect(sendCurrentTrackState, SIGNAL(entered()), this, SLOT(sendCurrentTrack()));
    connect(sendNextTrackState, SIGNAL(entered()), this, SLOT(sendNextTrack()));

    // Connect gui and actions
    connect(ui->usernameLineEdit, SIGNAL(returnPressed()), this, SLOT(submitLogin()));
    connect(ui->passwordLineEdit, SIGNAL(returnPressed()), this, SLOT(submitLogin()));
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(submitLogin()));
    connect(ui->nextButton, SIGNAL(clicked()), this, SLOT(nextTrack()));

    // Connect playlist and actions
    connect(player, SIGNAL(currentMediaChanged(QMediaContent)), this, SLOT(updateTrackLabels()));
    connect(this, SIGNAL(currentTrackSet()), this, SLOT(updateTrackLabels()));
    connect(this, SIGNAL(nextTrackSet()), this, SLOT(updateTrackLabels()));

    // Start state machine
    mainStateMachine->start();
    playerStateMachine->start();
}

SmoozikSimplestClientWindow::~SmoozikSimplestClientWindow()
{
    smoozikPlaylistFiller->abort();
    smoozikPlaylistFillerThread->wait();
    delete smoozikPlaylistFillerThread;
    delete smoozikPlaylistFiller;
    delete smoozikPlaylist;

    delete ui;
}

int SmoozikSimplestClientWindow::getCurrentTrackIndex()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#else
    if (playlist.mediaCount() >= 1) {
        if ((player->state() == QMediaPlayer::PausedState || player->state() == QMediaPlayer::PlayingState) && playlist.currentIndex() >= 0 ) {
            return smoozikPlaylist->indexOf(player->currentMedia().canonicalUrl().toLocalFile());
        } else {
            return smoozikPlaylist->indexOf(playlist.media(0).canonicalUrl().toLocalFile());
        }
    } else {
        return -1;
    }
#endif
}

int SmoozikSimplestClientWindow::getNextTrackIndex()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#else
    if (playlist.mediaCount() >= 2 && playlist.mediaCount() > playlist.currentIndex() + 1) {
        if ((player->state() == QMediaPlayer::PausedState || player->state() == QMediaPlayer::PlayingState) && playlist.currentIndex() >= 0) {
            return smoozikPlaylist->indexOf(playlist.media(playlist.currentIndex() + 1).canonicalUrl().toLocalFile());
        } else {
            return smoozikPlaylist->indexOf(playlist.media(1).canonicalUrl().toLocalFile());
        }
    } else {
        return -1;
    }
#endif
}


void SmoozikSimplestClientWindow::processNetworkReply(QNetworkReply *reply)
{
    SmoozikXml xml(reply);
    if (xml.error() != 0) {
        loginError(xml.errorMsg());
    } else {

        // Process different cases of request
        switch(state()) {

        case Login : {
            //Retrieve sessionKey
            smoozikManager->setSessionKey(xml["sessionKey"].toString());
            emit loggedIn();
            break;
        }

        case StartParty : {
            emit partyStarted();
            break;
        }

        case SendPlaylist : {
            emit playlistSent();
            break;
        }

        case GetTopTracks : {
            qDebug()<<"topTracksRetrieved";

            // Set mediaPlaylist from top tracks.
            SmoozikPlaylist topTracksPlaylist(xml["tracks"].toList());

            qDebug()<<"toptracks";
            qDebug()<<"n°1"<<topTracksPlaylist.value(0)->name();
            qDebug()<<"n°2"<<topTracksPlaylist.value(1)->name();
            qDebug()<<"n°3"<<topTracksPlaylist.value(2)->name();

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            while( i < topTracksPlaylist.size()  && (playlist.count() < 2 || playlistCurrentIndex > playlist.count() - 1)) {
                playlist.append(Phonon::MediaSource(topTracksPlaylist.value(i)->localId()));
                i++;
            }
            player->clearQueue();
            player->enqueue(playlist.mid(playlistCurrentIndex));
#else
            qDebug()<<"currentIndex"<<playlist.currentIndex()<<"mediaCount"<<playlist.mediaCount();
            for (int i = -1 ; i < playlist.mediaCount() ; i++) {
                if(!playlist.media(i).isNull()) {
                    qDebug()<<i<<playlist.media(i).canonicalUrl().toLocalFile();
                }
            }
            if (playlist.mediaCount() < 2 || playlist.currentIndex() >= playlist.mediaCount() - 2) {

                playlist.addMedia(QUrl::fromLocalFile(topTracksPlaylist.value(0)->localId()));
                if(playlist.mediaCount() <= 1 || playlist.currentIndex() == playlist.mediaCount() - 1) {
                    qDebug()<<"add current track";
                    emit currentTrackSet();
                } else {
                    qDebug()<<"add next track";
                    emit nextTrackSet();
                }
            }
#endif
            break;
        }

        case SendCurrentTrack : {
            qDebug()<<"currentTrackSent";
            emit currentTrackSent();
            break;
        }

        case SendNextTrack : {
            qDebug()<<"nextTrackSent";
            emit nextTrackSent();
            break;
        }

        default :
            loginError(tr("Unkown state encountered."));
        }
    }
}

void SmoozikSimplestClientWindow::submitLogin()
{
    ui->loginButton->setEnabled(false);
    ui->usernameLineEdit->setEnabled(false);
    ui->passwordLineEdit->setEnabled(false);
    ui->loginStateLabel->setText(tr("Connecting..."));
    qApp->processEvents();
    QString username = ui->usernameLineEdit->text();
    QString password = ui->passwordLineEdit->text();
    smoozikManager->login(username, password);
}

void SmoozikSimplestClientWindow::loginError(QString errorMsg)
{
    emit disconnect();
    ui->loginButton->setEnabled(true);
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginStateLabel->setText(errorMsg);
}

void SmoozikSimplestClientWindow::sendCurrentTrack()
{
    qDebug()<<"sendCurrentTrack";
    int currentTrackIndex = getCurrentTrackIndex();
    if (currentTrackIndex >= 0 && currentTrackIndex < smoozikPlaylist->count()) {
        SmoozikTrack *currentTrack = smoozikPlaylist->value(currentTrackIndex);

        if (currentTrack->localId().isEmpty()) {
            loginError("Current track could not be found in playlist.");
            return;
        }

        smoozikManager->setTrack(currentTrack, 0);
    }
}

void SmoozikSimplestClientWindow::sendNextTrack()
{
    qDebug()<<"sendNextTrack";
    // If player is not started yet, it is a good time to do it
    if (player->state() == QMediaPlayer::StoppedState) {
        player->play();
    }

    int nextTrackIndex = getNextTrackIndex();
    if (nextTrackIndex >= 0 && nextTrackIndex < smoozikPlaylist->count()) {
        SmoozikTrack *nextTrack = smoozikPlaylist->value(nextTrackIndex);

        if (nextTrack->localId().isEmpty()) {
            loginError("Next track could not be found in playlist.");
            return;
        }

        smoozikManager->setTrack(nextTrack, 1);
    }
}

void SmoozikSimplestClientWindow::retrieveTracksDialog()
{
    smoozikPlaylist->clear();
    QDir directory(_dirName);

    QFileDialog dialog(0, tr("Select folder"), directory.path(), "Directories");
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly, true);

    if (dialog.exec()) {
        _dirName = dialog.selectedFiles().value(0);

        //Start SmoozikPlaylist filler in another thread
        smoozikPlaylistFiller->abort();
        smoozikPlaylistFillerThread->wait();
        smoozikPlaylistFiller->setDirName(_dirName);
        smoozikPlaylistFillerThread->start();

    } else {
        emit disconnect();
    }
}

void SmoozikSimplestClientWindow::updateTrackLabels()
{
    qDebug()<<"update labels";

    int currentTrackIndex = getCurrentTrackIndex();
    if (currentTrackIndex >= 0 && currentTrackIndex < smoozikPlaylist->count()) {
        SmoozikTrack *currentTrack = smoozikPlaylist->value(currentTrackIndex);
        ui->currentTrackLabel->setText(QString("%1 - %2").arg(currentTrack->name()).arg(currentTrack->artist()));
    } else {
        ui->currentTrackLabel->setText(QString(" - "));
    }

    int nextTrackIndex = getNextTrackIndex();
    if (nextTrackIndex >= 0 && nextTrackIndex < smoozikPlaylist->count()) {
        SmoozikTrack *nextTrack = smoozikPlaylist->value(nextTrackIndex);
        ui->nextTrackLabel->setText(QString("%1 - %2").arg(nextTrack->name()).arg(nextTrack->artist()));
    } else {
        ui->nextTrackLabel->setText(QString(" - "));
    }
}

void SmoozikSimplestClientWindow::playerStateChanged(const Phonon::State newstate, const Phonon::State)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (newstate == Phonon::PlayingState) {
        emit playing();
    } else if (newstate == Phonon::PausedState) {
        emit paused();
    }
#else
    (void)newstate;
#endif
}

void SmoozikSimplestClientWindow::playerStateChanged(const QMediaPlayer::State state)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    (void)state;
#else
    if (state == QMediaPlayer::PlayingState) {
        emit playing();
    } else if (state == QMediaPlayer::PausedState) {
        emit paused();
    }
#endif
}

void SmoozikSimplestClientWindow::maxPlaylistSizeReachedMessage()
{
    QMessageBox messageBox(QMessageBox::Warning, tr("Max playlist size reached."), tr("Max playlist size of %1 tracks has been reached. Not all tracks were added to the playlist.").arg(MAX_ADVISED_PLAYLIST_SIZE));
    messageBox.exec();
}

void SmoozikSimplestClientWindow::noTrackRetrievedMessage()
{
    QMessageBox messageBox(QMessageBox::Warning, tr("No valid track in this directory."), tr("No valid track in this directory."));
    messageBox.exec();

    retrieveTracksDialog();
}

void SmoozikSimplestClientWindow::nextTrack()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#else
    // Check if there is a next track in playlist
    if (playlist.mediaCount() >= 2 && playlist.currentIndex() <= playlist.mediaCount() - 2) {
        playlist.next();
    }
#endif
}
