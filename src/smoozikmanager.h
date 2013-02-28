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

#ifndef SMOOZIKMANAGER_H
#define SMOOZIKMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QTime>
#include <QEventLoop>
#include <QDebug>
#include <QMap>
#include <QStringList>
#include <QCryptographicHash>
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QUrl>
#else
#include <QUrlQuery>
#endif

/**
 * @brief The SmoozikManager class is a Network Access Manager designed to send request to Smoozik server.
 */
class SmoozikManager : public QNetworkAccessManager {
    Q_OBJECT
    /**
     * @brief This property holds the apiKey of the client application.
     *
     * The apiKey is required for every request to Smoozik server. If you do not have any apiKey, you should inquery for one on Smoozik website : http://smoozik.com.
     * @af apiKey(), setApiKey()
     * @pm _apiKey
     */
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey)
    /**
     * @brief This property holds the secret string used to signed requests.
     *
     * The secret is required for every request to Smoozik server with access rights. If you do not have any secret string, you should inquery for one on Smoozik website : http://smoozik.com.
     * @af secret(), setSecret()
     * @pm _secret
     */
    Q_PROPERTY(QString secret READ secret WRITE setSecret)
    /**
     * @brief This property holds the session key used to identify on Smoozik server.
     *
     * The session key is retrieved using login() method.
     * @af sessionKey(), setSessionKey()
     * @pm _sessionKey
     */
    Q_PROPERTY(QString sessionKey READ sessionKey WRITE setSessionKey)
    Q_ENUMS(Format)
    /**
     * @brief This property holds the format the server responses are expected on.
     *
     * Currently two formats are supported: xml and json
     * @af format(), setFormat()
     * @pm _format
     */
    Q_PROPERTY(Format format READ format WRITE setFormat)
    /**
     * @brief This property holds wether requests should be blocking or not.
     *
     * A blocking request will return only once the server answer. A non-blocking request will return immediatly and the reply should be catch within a connected slot.
     * @af blocking(), setBlocking()
     * @pm _blocking
     */
    Q_PROPERTY(bool blocking READ blocking WRITE setBlocking)

public:
    enum Format {
        XML,
        JSON
    }; /**< @see #format */
    explicit SmoozikManager(const QString &apiKey, QObject *parent = 0, const QString &secret = "", const Format &format = XML, bool blocking = true);

    inline QString apiKey() const {
        return _apiKey;
    } /**< @see #apiKey */

    inline void setApiKey(const QString &apiKey) {
        _apiKey = apiKey;
    } /**< @see #apiKey */

    inline QString secret() const {
        return _secret;
    } /**< @see #secret */

    inline void setSecret(const QString &secret) {
        _secret = secret;
    } /**< @see #secret */

    inline QString sessionKey() const {
        return _sessionKey;
    } /**< @see #sessionKey */

    inline void setSessionKey(const QString &sessionKey) {
        _sessionKey = sessionKey;
    } /**< @see #sessionKey */

    inline Format format() const {
        return _format;
    } /**< @see #format */

    inline void setFormat(const Format &format) {
        _format = format;
    } /**< @see #format */

    inline bool blocking() const {
        return _blocking;
    } /**< @see #blocking */

    inline void setBlocking(bool blocking) {
        _blocking = blocking;
    } /**< @see #blocking */

    /**
     * @name API Methods
     */
    //@{
    /**
     * @brief Starts a party
     *
     * @rights Managers only
     */
    QNetworkReply *login(const QString &username, const QString &password);

    /**
     * @brief Starts a party
     *
     * @rights Managers only
     */
    QNetworkReply *startParty();

    /**
     * @brief Retrieves top tracks for the party
     * @param retrieve Max number of tracks to retrieve
     * @param retrieved Offset
     * @rights Managers only
     */
    QNetworkReply *getTopTracks(int retrieve = 10, int retrieved = 0);

    /**
     * @brief Sets a track for the party
     *
     * If this track has already been sent (with setTrack() or sendPlaylist()), the localId is the only required field.
     * @param localId Id of the track in client local database
     * @param position Position of the track in playlist
     * @param actual Is the track going to be actually played or is it still possible to change it
     * @param name Name of the track
     * @param artistName Name of the artist of the track
     * @param albumName Name of the album of the track
     * @rights Managers only
     */
    QNetworkReply *setTrack(const QString &localId, int position = 0, bool actual = true, const QString &name = "", const QString &artistName = "", const QString &albumName = "");

    /**
     * @brief Sends the playlist
     * @param data Playlist in #format format. The structure should respect standards defined in Smoozik API specification.
     * @rights Managers only
     */
    QNetworkReply *sendPlaylist(const QString &data);
    //@}

signals:
    /**
     * @brief This signal is emitted when the request is finished.
     * @param reply The network reply
     */
    void requestFinished(QNetworkReply *reply);

private:
    QString _apiKey; /**< @see #apiKey */
    QString _secret; /**< @see #secret */
    QString _sessionKey; /**< @see #sessionKey */
    Format _format; /**< @see #format */
    bool _blocking; /**< @see #blocking */
    /**
     * @brief Sends a signed request to Smoozik server using #apiKey and #sessionKey with #format.
     * @param method The requested methods
     * @param getParams Parameters sent through GET method
     * @param postParams Parameters sent through POST method
     * @return Reply of the server
     */
    QNetworkReply *request(const QString &method, QMap<QString, QString> getParams = QMap<QString, QString>(), QMap<QString, QString> postParams = QMap<QString, QString>());
};

#endif // SMOOZIKMANAGER_H
