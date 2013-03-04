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
#include "global.h"

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
     * Currently two formats are supported: xml and json. Currently, data must be sent in xml format.
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
    Q_ENUMS(Error)

public:
    /**
     * @brief The Error enum defines available formats to communicate with Smoozik server.
     * @sa #format
     */
    enum Format {
        XML,
        JSON
    };

    /**
     * @brief The Error enum defines error encountered during request.
     * @sa SmoozikXml::error
     */
    enum Error {
        ServerUnreachable = -2, /**< Server unreachable.*/
        ParseError = -1, /**< Parse error: An error occured while parsing received data. This error is not sent by server. It is generated by the parser.*/
        NoError = 0, /**< No error */
        ServiceFailed = 1, /**< Service failed: An error occured while processing method */
        InvalideMethod = 2, /**< Invalid method: The requested method does not exist */
        AuthenticationFailed = 3, /**< Authentication Failed */
        AuthenticationRequired = 4, /**< Authentication required: You must authentify before accessing this service */
        InvalidAPIKey = 5, /**< Invalid API key: Your client is not known by Smoozik, please register it */
        InvalidSignature = 6, /**< Invalid signature */
        ParameterMissing = 7, /**< Parameter missing: One or more parameter is missing for this method */
        InvalidFormat = 8, /**< Invalid format: This format is not currently implemented */
        AccessRestricted = 9, /**< Access restricted: You do not have the rights to access this service */
        InvalidSessionKey = 10, /**< Invalid Session key: Please re-authenticate */
        InvalidPartyId = 11, /**< Invalid Party Id: Please re-join this party */
        PartyDoesNotExist = 12, /**< Party does not exist: The requested party does not exist */
        InvalidCommand = 13, /**< Invalid Command: Command can only be 1 for like and 0 for dislike */
        TrackNotInParty = 14, /**< Track not in party: The requested track is not present in current party */
        UsernameAlreadyInUse = 15, /**< Username already in use: This error occures when a user try to sign in with a username already taken */
        MailAddressAlreadyInUse = 16, /**< Mail address already in use: This error occures when a user try to sign in with a mail address already taken */
        InvalidMailAddress = 17, /**< Invalid mail address */
        PartyHasNoTrack = 18, /**< Party has no track */
        SubscriptionOver = 19, /**< Your subscription is over */
        CannotParseSentData = 20 /**< Cannot parse sent data */
    };
    SMOOZIKLIB_EXPORT explicit SmoozikManager(const QString &apiKey, QObject *parent = 0, const QString &secret = "", const Format &format = XML, bool blocking = true);

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
     * @brief Logs user in.
     *
     * @rights Managers only
     */
    SMOOZIKLIB_EXPORT QNetworkReply *login(const QString &username, const QString &password);

    /**
     * @brief Starts a party.
     *
     * @rights Managers only
     */
    SMOOZIKLIB_EXPORT QNetworkReply *startParty();

    /**
     * @brief Retrieves top tracks for the party.
     * @param retrieve Max number of tracks to retrieve
     * @param retrieved Offset
     * @rights Managers only
     */
    SMOOZIKLIB_EXPORT QNetworkReply *getTopTracks(int retrieve = 10, int retrieved = 0);

    /**
     * @brief Sets a track for the party.
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
    SMOOZIKLIB_EXPORT QNetworkReply *setTrack(const QString &localId, int position = 0, bool actual = true, const QString &name = "", const QString &artistName = "", const QString &albumName = "");

    /**
     * @brief Sends the playlist.
     * @param data Playlist in XML format. The structure should respect standards defined in Smoozik API specification.
     * @rights Managers only
     */
    SMOOZIKLIB_EXPORT QNetworkReply *sendPlaylist(const QString &data);
    //@}

    /**
     * @brief Sends a signed request to Smoozik server using #apiKey and #sessionKey with #format.
     * @param method The requested methods
     * @param getParams Parameters sent through GET method
     * @param postParams Parameters sent through POST method
     * @return Reply of the server
     */
    SMOOZIKLIB_EXPORT QNetworkReply *request(const QString &method, QMap<QString, QString> getParams = QMap<QString, QString>(), QMap<QString, QString> postParams = QMap<QString, QString>());

private:
    QString _apiKey; /**< @see #apiKey */
    QString _secret; /**< @see #secret */
    QString _sessionKey; /**< @see #sessionKey */
    Format _format; /**< @see #format */
    bool _blocking; /**< @see #blocking */

signals:
    /**
     * @brief This signal is emitted when the request is finished.
     * @param reply The network reply
     */
    void requestFinished(QNetworkReply *reply);
};

#endif // SMOOZIKMANAGER_H
