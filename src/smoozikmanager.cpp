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
QNetworkAccessManager(parent) {
    setApiKey(apiKey);
    setSecret(QString());
    setFormat(format);
    setBlocking(blocking);
}

SmoozikManager::SmoozikManager(const QString &apiKey, const QString &secret, const SmoozikManager::Format &format, bool blocking, QObject *parent) :
QNetworkAccessManager(parent) {
    setApiKey(apiKey);
    setSecret(secret);
    setFormat(format);
    setBlocking(blocking);
}

QNetworkReply *SmoozikManager::login(const QString &username, const QString &password) {
    QMap<QString, QString> postParams;
    postParams.insert("username", username);
    postParams.insert("authHash", QCryptographicHash::hash(username.toLatin1() + QCryptographicHash::hash(password.toLatin1(), QCryptographicHash::Md5).toHex(), QCryptographicHash::Md5).toHex());

    return request("login", QMap<QString, QString>(), postParams);
}

QNetworkReply *SmoozikManager::startParty() {
    return request("startParty");
}

QNetworkReply *SmoozikManager::getTopTracks(int retrieve, int retrieved) {
    QMap<QString, QString> getParams;
    getParams.insert("retrieve", QString::number(retrieve));
    getParams.insert("retrieved", QString::number(retrieved));

    return request("getTopTracks", getParams);
}

QNetworkReply *SmoozikManager::setTrack(const QString &localId, const QString &name, int position, const QString &artistName, const QString &albumName, uint duration) {
    QMap<QString, QString> postParams;
    postParams.insert("localId", localId);
    postParams.insert("name", name);
    postParams.insert("position", QString::number(position));
    postParams.insert("artistName", artistName);
    postParams.insert("albumName", albumName);
    postParams.insert("duration", QString::number(duration));

    return request("setTrack", QMap<QString, QString>(), postParams);
}

QNetworkReply *SmoozikManager::sendPlaylist(const SmoozikPlaylist *playlist) {
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

QNetworkReply *SmoozikManager::request(const QString &method, QMap<QString, QString> getParams, QMap<QString, QString> postParams) {
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
            postData.addQueryItem(i.key(), i.value());
            if (i.key() != "sig")
                keyList << i.key();
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
            getData.addQueryItem(j.key(), j.value());
            if (j.key() != "sig")
                keyList << j.key();
        }
    }

    //Process signature
    keyList.sort();
    QByteArray sigParams;
    for (int k = 0; k < keyList.size(); k++) {
        QByteArray key = keyList.value(k).toUtf8();
        sigParams += key + getParams[key].toUtf8() + postParams[key].toUtf8();
    }

    QString sig = QCryptographicHash::hash(sigParams + secret().toUtf8(), QCryptographicHash::Md5).toHex();
    postData.addQueryItem("sig", sig);

    //Define method url
    QUrl pageUrl = QUrl("http://www.smoozik.com/index.php/api/" + method);

    //Construct request
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    request.setUrl(pageUrl.toString() + "?" + getData.encodedQuery());
#else
    request.setUrl(pageUrl.toString() + "?" + getData.query(QUrl::FullyEncoded).toUtf8());
#endif

    QEventLoop loop;
    connect(this, SIGNAL(finished(QNetworkReply*)), &loop, SLOT(quit()));
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    QNetworkReply *reply = post(request, postData.encodedQuery());
#else
    QNetworkReply *reply = post(request, postData.query(QUrl::FullyEncoded).toUtf8());
#endif
    if (blocking()) {
        loop.exec();
    }
    return reply;
}
