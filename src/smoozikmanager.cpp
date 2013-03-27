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

#include "smoozikmanager.h"

SmoozikManager::SmoozikManager(const QString &apiKey, const SmoozikManager::Format &format, bool blocking, QObject *parent) :
    QNetworkAccessManager(parent)
{
    setApiKey(apiKey);
    setSecret(QString());
    setFormat(format);
    setBlocking(blocking);
}

SmoozikManager::SmoozikManager(const QString &apiKey, const QString &secret, const SmoozikManager::Format &format, bool blocking, QObject *parent) :
    QNetworkAccessManager(parent)
{
    setApiKey(apiKey);
    setSecret(secret);
    setFormat(format);
    setBlocking(blocking);
}

QNetworkReply *SmoozikManager::login(const QString &username, const QString &password)
{
    QMap<QString, QString> postParams;
    postParams.insert("username", username);
    postParams.insert("authHash", QCryptographicHash::hash(username.toLatin1() + QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Md5).toHex(), QCryptographicHash::Md5).toHex());

    return request("login", QMap<QString, QString>(), postParams);
}

QNetworkReply *SmoozikManager::startParty()
{
    return request("startParty");
}

QNetworkReply *SmoozikManager::getTopTracks(int retrieve, int retrieved)
{
    QMap<QString, QString> getParams;
    getParams.insert("retrieve", QString::number(retrieve));
    getParams.insert("retrieved", QString::number(retrieved));

    return request("getTopTracks", getParams);
}

QNetworkReply *SmoozikManager::setTrack(const QString &localId, const QString &name, const QString &artistName, const QString &albumName, uint duration, int position)
{
    QMap<QString, QString> postParams;
    postParams.insert("localId", localId);
    postParams.insert("name", name);
    postParams.insert("artistName", artistName);
    postParams.insert("albumName", albumName);
    postParams.insert("duration", QString::number(duration));
    postParams.insert("position", QString::number(position));

    return request("setTrack", QMap<QString, QString>(), postParams);
}

QNetworkReply *SmoozikManager::unsetTrack(const QString &localId)
{
    QMap<QString, QString> postParams;
    postParams.insert("localId", localId);

    return request("unsetTrack", QMap<QString, QString>(), postParams);
}

QNetworkReply *SmoozikManager::unsetAllTracks()
{
    return request("unsetAllTracks");
}

QNetworkReply *SmoozikManager::sendPlaylist(const SmoozikPlaylist *playlist)
{
    QDomDocument doc;
    QDomElement partytracksElement = doc.createElement("partytracks");
    doc.appendChild(partytracksElement);

    for (int i = 0; i < playlist->size(); i++) {
        QDomElement partytrackElement = doc.createElement("partytrack");
        partytracksElement.appendChild(partytrackElement);

        // Retrieve localID
        QDomElement localId = doc.createElement("localId");
        partytrackElement.appendChild(localId);
        localId.appendChild(doc.createTextNode(playlist->value(i)->localId()));

        QDomElement trackElement = doc.createElement("track");
        partytrackElement.appendChild(trackElement);

        // Retrieve track name
        QDomElement name = doc.createElement("name");
        trackElement.appendChild(name);
        name.appendChild(doc.createTextNode(playlist->value(i)->name()));

        // Retrieve track artist
        if (!playlist->value(i)->artist().isEmpty()) {
            QDomElement artist = doc.createElement("artistName");
            trackElement.appendChild(artist);
            artist.appendChild(doc.createTextNode(playlist->value(i)->artist()));
        }

        // Retrieve track album
        if (!playlist->value(i)->album().isEmpty()) {
            QDomElement album = doc.createElement("albumName");
            trackElement.appendChild(album);
            album.appendChild(doc.createTextNode(playlist->value(i)->album()));
        }

        //Retrieve track duration
        if (playlist->value(i)->duration() > 0) {
            QDomElement duration = doc.createElement("duration");
            partytrackElement.appendChild(duration);
            duration.appendChild(doc.createTextNode(QString::number(playlist->value(i)->duration())));
        }
    }

    QString data = doc.toString();
    QMap<QString, QString> postParams;
    postParams.insert("data", data);

    return request("sendPlaylist", QMap<QString, QString>(), postParams);
}

QNetworkReply *SmoozikManager::request(const QString &method, QMap<QString, QString> getParams, QMap<QString, QString> postParams)
{
    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    //Add format
    switch (format()) {
    case XML:
        getParams.insert("format", "xml");
        break;
    case JSON:
        getParams.insert("format", "json");
        break;
    }

    //Add key
    postParams.insert("apiKey", apiKey());

    //Add sessionKey
    postParams.insert("sessionKey", sessionKey());

    //Process signature
    QStringList keyList;
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QUrl postData;
#else
    QUrlQuery postData;
#endif
    QMapIterator<QString, QString> i(postParams);
    while (i.hasNext()) {
        i.next();
        if (!i.value().isEmpty()) {
            QString key = i.key();
            QString value = i.value();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            //Percent characters are encoded manually as they are not encoded by Qt encode function.
            key.replace(QByteArray("%"), QByteArray("%25"));
            value.replace(QByteArray("%"), QByteArray("%25"));
#endif
            postData.addQueryItem(key, value);
            if (i.key() != "sig") {
                keyList << i.key();
            }
        }
    }

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QUrl getData;
#else
    QUrlQuery getData;
#endif
    QMapIterator<QString, QString> j(getParams);
    while (j.hasNext()) {
        j.next();
        if (!j.value().isEmpty()) {
            QString key = j.key();
            QString value = j.value();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            //Percent characters are encoded manually as they are not encoded by Qt encode function.
            key.replace(QByteArray("%"), QByteArray("%25"));
            value.replace(QByteArray("%"), QByteArray("%25"));
#endif
            getData.addQueryItem(key, value);
            if (j.key() != "sig") {
                keyList << j.key();
            }
        }
    }

    //Process signature
    keyList.sort();
    QByteArray sigParams;
    for (int k = 0; k < keyList.size(); k++) {
        QByteArray key = keyList.value(k).toUtf8();
        sigParams += key + getParams[key].toUtf8() + postParams[key].toUtf8();
    }

    QCryptographicHash hash(QCryptographicHash::Md5);
    hash.addData(sigParams + secret().toUtf8());
    QString sig = hash.result().toHex();
    postData.addQueryItem("sig", sig);

    //Define method url
    QUrl pageUrl = QUrl("http://www.smoozik.com/index.php/api/" + method);

    //Encode data
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QByteArray encodedGetData = getData.encodedQuery().replace(QByteArray("+"), QByteArray("%2B"));
    QByteArray encodedPostData = postData.encodedQuery().replace(QByteArray("+"), QByteArray("%2B"));
#else
    QByteArray encodedGetData = getData.query(QUrl::FullyEncoded).toUtf8().replace(QByteArray("+"), QByteArray("%2B"));
    QByteArray encodedPostData = postData.query(QUrl::FullyEncoded).toUtf8().replace(QByteArray("+"), QByteArray("%2B"));
#endif

    //Construct request
    request.setUrl(pageUrl.toString() + "?" + encodedGetData);

    QEventLoop loop;
    connect(this, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
    QNetworkReply *reply = post(request, encodedPostData);
    if (blocking()) {
        loop.exec();
    }
    return reply;
}
