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
