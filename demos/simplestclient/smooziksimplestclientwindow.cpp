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
#include "fileref.h"
#include "tag.h"

#include <QStateMachine>
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>

SmoozikSimplestClientWindow::SmoozikSimplestClientWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SmoozikSimplestClientWindow)
{
    ui->setupUi(this);

    // Initialize SmoozikManager
    smoozikManager = new SmoozikManager(APIKEY, SECRET, SmoozikManager::XML, false, this);
    connect(smoozikManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(processNetworkReply(QNetworkReply*)));

    // Initialize player
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    player = new Phonon::MediaObject(this);
    Phonon::AudioOutput *audioOutput = new Phonon::AudioOutput(Phonon::MusicCategory, this);
    Phonon::createPath(player, audioOutput);
    playlistCurrentIndex = 0;
    connect(player, SIGNAL(currentSourceChanged(Phonon::MediaSource)), this, SLOT(updatePlaylistCurrentIndex(Phonon::MediaSource)));
#else
    player = new QMediaPlayer(this);
    playlist.setPlaybackMode(QMediaPlaylist::Sequential);
    player->setPlaylist(&playlist);
#endif

    // Initialize main state machine which controls what is displayed
    QStateMachine *stateMachine = new QStateMachine(this);
    QState *loginState = new QState(stateMachine);
    QState *retrieveTracksState = new QState(stateMachine);
    QState *playerState = new QState(stateMachine);
    QState *playingState = new QState(playerState);
    QState *pausedState = new QState(playerState);

    // Define state transitions
    stateMachine->setInitialState(loginState);
    loginState->addTransition(this, SIGNAL(ready()), retrieveTracksState);
    retrieveTracksState->addTransition(this, SIGNAL(disconnect()), loginState);

    // Define state properties
    loginState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->loginPage));
    loginState->assignProperty(ui->loginButton, "enabled", true);
    loginState->assignProperty(ui->usernameLineEdit, "enabled", true);
    loginState->assignProperty(ui->passwordLineEdit, "enabled", true);
    loginState->assignProperty(ui->loginStateLabel, "text", QString());

    retrieveTracksState->assignProperty(ui->stackedWidget, "currentIndex", ui->stackedWidget->indexOf(ui->loadingPage));
    retrieveTracksState->assignProperty(ui->loadingLabel, "text", tr("Retrieving tracks..."));

    // Connect gui and actions
    connect(ui->usernameLineEdit, SIGNAL(returnPressed()), this, SLOT(submitLogin()));
    connect(ui->passwordLineEdit, SIGNAL(returnPressed()), this, SLOT(submitLogin()));
    connect(ui->loginButton, SIGNAL(clicked()), this, SLOT(submitLogin()));

    // Start state machine
    stateMachine->start();
}

SmoozikSimplestClientWindow::~SmoozikSimplestClientWindow()
{
    delete ui;
}

int SmoozikSimplestClientWindow::addTracksToPlaylist(const QDir *directory, SmoozikPlaylist *playlist)
{

    foreach(QString fileName, directory->entryList()) {

        if (fileName != ".." && fileName != ".") {
            QString fullPathFileName = directory->filePath(fileName);

            QFileInfo fileInfo(fullPathFileName);

            // If it is a directory, use addTracksToPlaylist recursively.
            if (fileInfo.isDir()) {
                QDir subdirectory(fullPathFileName);
                if (addTracksToPlaylist(&subdirectory, playlist) == -1) {
                    return -1;
                }
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
                        playlist->addTrack(fullPathFileName, name, artist, album);

                        if (playlist->size() >= MAX_ADVISED_PLAYLIST_SIZE) {

                            QMessageBox messageBox(QMessageBox::Warning, tr("Max playlist size reached."), tr("Max playlist size of %1 tracks has been reached. Not all tracks were added to the playlist.").arg(MAX_ADVISED_PLAYLIST_SIZE));
                            messageBox.exec();
                            return -1;
                        }
                    }
                }
            }
        }
    }

    return 0;

}

void SmoozikSimplestClientWindow::processNetworkReply(QNetworkReply *reply)
{
    QString path = reply->url().path();
    SmoozikXml xml(reply);
    // Process different cases of request

    //Login case
    if (path.endsWith("api/login", Qt::CaseInsensitive)) {
        if (xml.error() == 0) {

            //Retrieve sessionKey
            smoozikManager->setSessionKey(xml["sessionKey"].toString());

            // Starting party
            ui->loginStateLabel->setText(tr("Starting party..."));
            qApp->processEvents();
            smoozikManager->startParty();
        } else {
            loginError(xml.errorMsg());
        }
    }

    //Start party case
    else if (path.endsWith("api/startParty", Qt::CaseInsensitive)) {
        if (xml.error() == 0) {
            ui->loginStateLabel->setText(tr("Connected"));
            emit ready();
            retrieveTracksDialog();
        } else {
            loginError(xml.errorMsg());
        }
    }

    //Send playlist case
    else if (path.endsWith("api/sendPlaylist", Qt::CaseInsensitive)) {
        if (xml.error() == 0) {
            ui->loadingLabel->setText(tr("Get top tracks..."));
            qApp->processEvents();
            smoozikManager->getTopTracks();
        } else {
            loginError(xml.errorMsg());
        }
    }

    //Get top tracks case
    else if (path.endsWith("api/getTopTracks", Qt::CaseInsensitive)) {
        if (xml.error() == 0) {
            // Set mediaPlaylist from top tracks.
            SmoozikPlaylist topTracksPlaylist(xml["tracks"].toList());

            int i = 0;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            while( i < topTracksPlaylist.size()  && (playlist.count() < 2 || playlistCurrentIndex > playlist.count() - 1)) {
                playlist.append(Phonon::MediaSource(topTracksPlaylist.value(i)->localId()));
                i++;
            }
            player->clearQueue();
            player->enqueue(playlist.mid(playlistCurrentIndex));
#else
            while( i < topTracksPlaylist.size()  && (playlist.mediaCount() < 2 || playlist.currentIndex() > playlist.mediaCount() - 1)) {
                playlist.addMedia(QUrl::fromLocalFile(topTracksPlaylist.value(i)->localId()));
                i++;
            }
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            if (playlist.count() < 2) {
#else
            if (playlist.mediaCount() < 2) {
#endif
                loginError(tr("A problem occured, the playlist could not be filled with enough playable tracks."));
            } else {
                player->play();
            }
        } else {
            loginError(xml.errorMsg());
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

void SmoozikSimplestClientWindow::retrieveTracksDialog()
{
    SmoozikPlaylist playlist;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QDir directory(QDesktopServices::storageLocation(QDesktopServices::MusicLocation));
#else
    QDir directory(QStandardPaths::writableLocation(QStandardPaths::MusicLocation));
#endif
    while (playlist.isEmpty()) {
        QFileDialog dialog(0, tr("Select folder"), directory.path(), "Directories");
        dialog.setFileMode(QFileDialog::Directory);
        dialog.setOption(QFileDialog::ShowDirsOnly, true);
        if (dialog.exec()) {
            directory = QDir(dialog.selectedFiles().value(0));

            addTracksToPlaylist(&directory, &playlist);

            if (playlist.isEmpty()) {
                QMessageBox messageBox(QMessageBox::Warning, tr("No valid track in this directory."), tr("No valid track in this directory."));
                messageBox.exec();
            }
        } else {
            emit disconnect();
            return;
        }
    }

    ui->loadingLabel->setText(tr("Sending playlist..."));
    qApp->processEvents();
    smoozikManager->sendPlaylist(&playlist);
}
