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
    connect(player, SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SLOT(updateTrackLabels()));
    connect(player, SIGNAL(stateChanged(Phonon::State,Phonon::State)), this, SLOT(playerStateChanged()));
#else
    player = new QMediaPlayer(this);
    player->setPlaylist(new QMediaPlaylist(player));
    player->playlist()->setPlaybackMode(QMediaPlaylist::Sequential);
    connect(player, SIGNAL(currentMediaChanged(QMediaContent)), this, SLOT(updateTrackLabels()));
    connect(player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStateChanged()));
#endif
    connect(ui->playButton, SIGNAL(clicked()), player, SLOT(play()));
    connect(ui->pauseButton, SIGNAL(clicked()), player, SLOT(pause()));
    connect(this, SIGNAL(currentTrackSet()), this, SLOT(updateTrackLabels()));
    connect(this, SIGNAL(nextTrackSet()), this, SLOT(updateTrackLabels()));

    // Initialize main state machine which controls what is displayed
    QStateMachine *mainStateMachine = new QStateMachine(this);
    QState *mainState = new QState(mainStateMachine);
    QState *loginState = new QState(mainState);
    QState *startPartyState = new QState(mainState);
    QState *connectedState = new QState(mainState);
    QState *retrieveTracksState = new QState(connectedState);
    QState *sendPlaylistState = new QState(connectedState);
    QState *getTopTracksState = new QState(connectedState);
    QState *partyState = new QState(connectedState);
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
    connectedState->setInitialState(retrieveTracksState);
    partyState->setInitialState(waitingState);
    playerStateMachine->setInitialState(playerState);
    playerState->setInitialState(pausedState);

    mainState->addTransition(this, SIGNAL(disconnected()), loginState);
    loginState->addTransition(this, SIGNAL(loggedIn()), startPartyState);
    startPartyState->addTransition(this, SIGNAL(partyStarted()), connectedState);
    connectedState->addTransition(ui->changePlaylistButton, SIGNAL(clicked()), retrieveTracksState);
    retrieveTracksState->addTransition(this, SIGNAL(tracksRetrieved()), sendPlaylistState);
    sendPlaylistState->addTransition(this, SIGNAL(playlistSent()), getTopTracksState);
    getTopTracksState->addTransition(this, SIGNAL(currentTrackSet()), sendCurrentTrackState);
    sendCurrentTrackState->addTransition(this, SIGNAL(currentTrackSent()), getTopTracksState);
    getTopTracksState->addTransition(this, SIGNAL(nextTrackSet()), sendNextTrackState);
    sendNextTrackState->addTransition(this, SIGNAL(nextTrackSent()), waitingState);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    waitingState->addTransition(player, SIGNAL(currentSourceChanged(Phonon::MediaSource)), sendCurrentTrackState);
#else
    waitingState->addTransition(player, SIGNAL(currentMediaChanged(QMediaContent)), sendCurrentTrackState);
#endif

    playerState->addTransition(this, SIGNAL(playing()), playingState);
    playerState->addTransition(this, SIGNAL(paused()), pausedState);

    // Define state properties
    loginState->assignProperty(this, "state", Login);
    loginState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->loginPage));
    loginState->assignProperty(ui->loginButton, "enabled", true);
    loginState->assignProperty(ui->disconnectButton, "visible", false);
    loginState->assignProperty(ui->changePlaylistButton, "visible", false);
    loginState->assignProperty(ui->usernameLineEdit, "enabled", true);
    loginState->assignProperty(ui->passwordLineEdit, "enabled", true);
    loginState->assignProperty(ui->loginStateLabel, "text", QString());

    startPartyState->assignProperty(this, "state", StartParty);
    startPartyState->assignProperty(ui->loginStateLabel, "text", tr("Starting party..."));
    startPartyState->assignProperty(ui->disconnectButton, "visible", false);
    startPartyState->assignProperty(ui->changePlaylistButton, "visible", false);

    connectedState->assignProperty(ui->disconnectButton, "visible", true);

    retrieveTracksState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->loadingPage));
    retrieveTracksState->assignProperty(ui->loginStateLabel, "text", tr("Connected"));
    retrieveTracksState->assignProperty(ui->loadingLabel, "text", tr("Retrieving tracks..."));
    retrieveTracksState->assignProperty(ui->changePlaylistButton, "visible", false);

    sendPlaylistState->assignProperty(this, "state", SendPlaylist);
    sendPlaylistState->assignProperty(ui->loadingLabel, "text", tr("Sending playlist..."));
    sendPlaylistState->assignProperty(ui->changePlaylistButton, "visible", true);

    getTopTracksState->assignProperty(this, "state", GetTopTracks);
    getTopTracksState->assignProperty(ui->loadingLabel, "text", tr("Get top tracks..."));
    getTopTracksState->assignProperty(ui->nextButton, "enabled", false);
    getTopTracksState->assignProperty(ui->changePlaylistButton, "visible", true);

    partyState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->playerPage));
    partyState->assignProperty(ui->changePlaylistButton, "visible", true);

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
    connect(ui->disconnectButton, SIGNAL(clicked()), this, SLOT(disconnect()));

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

void SmoozikSimplestClientWindow::getCurrentTrackInfo(QString *localId, QString *name, QString *artist, QString *album)
{
    *localId = QString();
    *name = QString();
    *artist = QString();
    *album = QString();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    *localId = player->currentSource().url().toLocalFile();
#else
    *localId = player->currentMedia().canonicalUrl().toLocalFile();
#endif
    TagLib::FileRef mediaFileRef(QFile::encodeName(*localId).constData());
    if (!mediaFileRef.isNull()) {
        *name = TStringToQString(mediaFileRef.tag()->title());
        *artist = TStringToQString(mediaFileRef.tag()->artist());
        *album = TStringToQString(mediaFileRef.tag()->album());
    }
}

void SmoozikSimplestClientWindow::getNextTrackInfo(QString *localId, QString *name, QString *artist, QString *album)
{
    *localId = QString();
    *name = QString();
    *artist = QString();
    *album = QString();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (!player->queue().isEmpty()) {
        *localId = player->queue().at(0).url().toLocalFile();
#else
    if (player->playlist()->currentIndex() >= 0 && player->playlist()->mediaCount() > player->playlist()->currentIndex() + 1) {
        *localId = player->playlist()->media(player->playlist()->currentIndex() + 1).canonicalUrl().toLocalFile();
#endif
        TagLib::FileRef mediaFileRef(QFile::encodeName(*localId).constData());
        if (!mediaFileRef.isNull()) {
            *name = TStringToQString(mediaFileRef.tag()->title());
            *artist = TStringToQString(mediaFileRef.tag()->artist());
            *album = TStringToQString(mediaFileRef.tag()->album());
        }
    }
}

void SmoozikSimplestClientWindow::processNetworkReply(QNetworkReply *reply)
{
    QString path = reply->url().path();
    SmoozikXml xml(reply);
    if (xml.error() != 0) {
        error(xml.errorMsg());
    } else {

        // Process different cases of request
        if (path.endsWith("login", Qt::CaseInsensitive) && state() == Login) {
            //Retrieve sessionKey
            smoozikManager->setSessionKey(xml["sessionKey"].toString());
            emit loggedIn();
        }

        else if (path.endsWith("startParty", Qt::CaseInsensitive) && state() == StartParty) {
            emit partyStarted();
        }

        else if (path.endsWith("sendPlaylist", Qt::CaseInsensitive) && state() == SendPlaylist) {
            emit playlistSent();
        }

        else if (path.endsWith("getTopTracks", Qt::CaseInsensitive) && state() == GetTopTracks) {

            // Set mediaPlaylist from top tracks.
            SmoozikPlaylist topTracksPlaylist(xml["tracks"].toList());

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            if (player->queue().isEmpty()) {

                if(player->currentSource().type() != Phonon::MediaSource::LocalFile) {
                    player->setCurrentSource(Phonon::MediaSource(topTracksPlaylist.value(0)->localId()));
                    emit currentTrackSet();
                } else {
                    player->enqueue(Phonon::MediaSource(topTracksPlaylist.value(0)->localId()));
                    emit nextTrackSet();
                }
            }
#else
            if (player->playlist()->mediaCount() < 2 || player->playlist()->currentIndex() >= player->playlist()->mediaCount() - 2) {

                player->playlist()->addMedia(QUrl::fromLocalFile(topTracksPlaylist.value(0)->localId()));
                if(player->playlist()->mediaCount() == 1 || player->playlist()->currentIndex() == player->playlist()->mediaCount() - 1) {
                    emit currentTrackSet();
                } else {
                    emit nextTrackSet();
                }
            }
#endif
        }

        else if (path.endsWith("setTrack", Qt::CaseInsensitive) && state() == SendCurrentTrack) {
            emit currentTrackSent();
        }

        else if (path.endsWith("setTrack", Qt::CaseInsensitive) && state() == SendNextTrack) {
            emit nextTrackSent();
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

void SmoozikSimplestClientWindow::disconnect()
{
    smoozikManager->setSessionKey(QString());
    smoozikPlaylist->clear();
    player->stop();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    player->clearQueue();
#else
    player->playlist()->clear();
#endif
    emit disconnected();
}

void SmoozikSimplestClientWindow::error(QString errorMsg)
{
    disconnect();
    ui->loginButton->setEnabled(true);
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
    ui->loginStateLabel->setText(errorMsg);
}

void SmoozikSimplestClientWindow::sendCurrentTrack()
{
    // If player is not started yet, it is a good time to do it
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (player->state() != Phonon::PlayingState && player->state() != Phonon::PausedState) {
#else
    if (player->state() != QMediaPlayer::PlayingState && player->state() != QMediaPlayer::PausedState) {
#endif
        player->play();
    }

    QString localId, name, artist, album;
    getCurrentTrackInfo(&localId, &name, &artist, &album);
    if (!localId.isEmpty() && !name.isEmpty()) {
        smoozikManager->setTrack(localId, name, artist, album, 0, 0);
    }
}

void SmoozikSimplestClientWindow::sendNextTrack()
{
    QString localId, name, artist, album;
    getNextTrackInfo(&localId, &name, &artist, &album);
    if (!localId.isEmpty() && !name.isEmpty()) {
        smoozikManager->setTrack(localId, name, artist, album, 0, 1);
    }
}

void SmoozikSimplestClientWindow::retrieveTracksDialog()
{
    smoozikPlaylist->clear();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    player->clearQueue();
#else
    for (int i = player->playlist()->currentIndex() + 1; i < player->playlist()->mediaCount();  i++) {
        player->playlist()->removeMedia(player->playlist()->currentIndex() + 1);
    }
#endif
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
        disconnect();
    }
}

void SmoozikSimplestClientWindow::updateTrackLabels()
{
    QString localId, name, artist, album;
    getCurrentTrackInfo(&localId, &name, &artist, &album);
    ui->currentTrackLabel->setText(QString("%1 - %2").arg(name).arg(artist));

    getNextTrackInfo(&localId, &name, &artist, &album);
    ui->nextTrackLabel->setText(QString("%1 - %2").arg(name).arg(artist));
}

void SmoozikSimplestClientWindow::playerStateChanged()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (player->state() == Phonon::PlayingState) {
#else
    if (player->state() == QMediaPlayer::PlayingState) {
#endif
        emit playing();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    } else if (player->state() == Phonon::PausedState) {
#else
    } else if (player->state() == QMediaPlayer::PausedState) {
#endif
        emit paused();
    }
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
    if (!player->queue().isEmpty()) {
        player->setCurrentSource(player->queue().value(0));
        player->play();
    }
#else
    if (player->playlist()->mediaCount() >= 2 && player->playlist()->currentIndex() <= player->playlist()->mediaCount() - 2) {
        player->playlist()->next();
    }
#endif
}
