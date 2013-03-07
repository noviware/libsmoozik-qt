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

#include "smoozikxml.h"

SmoozikXml::SmoozikXml(QObject *parent) :
QObject(parent),
QVariantMap() {
    cleanError();
}

void SmoozikXml::parse(const QDomElement &dataElement) {
    cleanError();
    clear();

    parseElement(dataElement, this);
}

bool SmoozikXml::parse(QNetworkReply *reply) {
    cleanError();

    QByteArray xmlReply = reply->readAll();
    reply->deleteLater();
    if (xmlReply.isEmpty()) {
        _error = SmoozikManager::ServerUnreachable;
        _errorMsg = tr("Could not reach server.");
        return false;
    }

    // Parse xml
    QDomDocument xml;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!xml.setContent(xmlReply, &errorMsg, &errorLine, &errorColumn)) {
        _error = SmoozikManager::ParseError;
        _errorMsg = tr("Could not parse xml : %1 (line: %2, column: %3).").arg(errorMsg).arg(errorLine).arg(errorColumn);
        return false;
    }

    QDomElement smoozikElement = xml.firstChildElement("smoozik");
    if (smoozikElement.isNull()) {
        _error = SmoozikManager::ParseError;
        _errorMsg = tr("Could not parse xml : %1 element is missing.").arg("smoozik");
        return false;
    }

    QDomElement statusElement = smoozikElement.firstChildElement("status");
    if (statusElement.isNull()) {
        _error = SmoozikManager::ParseError;
        _errorMsg = tr("Could not parse xml : %1 element is missing.").arg("status");
        return false;
    }

    if (statusElement.text() == "failed") {

        QDomElement errorElement = smoozikElement.firstChildElement("error");
        if (errorElement.isNull()) {
            _error = SmoozikManager::ParseError;
            _errorMsg = tr("Could not parse xml : %1 element is missing.").arg("failed");
            return false;
        }

        QDomElement codeElement = errorElement.firstChildElement("code");
        if (codeElement.isNull()) {
            _error = SmoozikManager::ParseError;
            _errorMsg = tr("Could not parse xml : %1 element is missing.").arg("code");
            return false;
        }
        int code = codeElement.text().toInt();
        QDomElement messageElement = errorElement.firstChildElement("message");
        QString message = messageElement.text();

        _error = (SmoozikManager::Error)code;
        _errorMsg = message;
        return false;
    }

    QDomElement dataElement = smoozikElement.firstChildElement("data");
    if (dataElement.isNull()) {
        _error = SmoozikManager::ParseError;
        _errorMsg = tr("Could not parse xml : %1 element is missing.").arg("data");
        return false;
    }

    parse(dataElement);
    return true;
}

QString SmoozikXml::print() {
    return "{\n" + printMap(this, 1) + "}\n";
}

void SmoozikXml::parseElement(const QDomElement &element, QVariantMap *map) {

    //Check if element is an array or an object
    bool array = false;
    QDomElement fe = element.firstChildElement();
    if (!fe.nextSiblingElement(fe.tagName()).isNull()) {
        array = true;
    }
    int i = 0;
    for (QDomElement e = element.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        QDomNode f = e.firstChild();
        if (!f.isNull()) {
            QDomText t = f.toText();
            if (!t.isNull()) {
                (*map)[e.tagName()] = t.data();
            } else {

                QVariantMap cmap;
                parseElement(e, &cmap);

                //Check if it is an array or an object
                if (array) {
                    QVariantMap vmap;
                    vmap[e.tagName()] = cmap;
                    (*map)[QString::number(i++)] = vmap;

                } else {
                    (*map)[e.tagName()] = cmap;
                }
            }
        }
    }
}

QString SmoozikXml::printMap(const QVariantMap *map, const int indent) {
    QString res;
    QString tab;
    for (int i = 0; i < indent; i++) {
        tab += "    ";
    }

    foreach(QString key, map->keys()) {
        const QVariant val = map->value(key);
        if (!val.toMap().isEmpty()) {
            const QVariantMap vmap = val.toMap();
            res += tab + key + " :\n" + tab + "{\n" + printMap(&vmap, indent + 1) + tab + "}\n";
        } else if (!val.toString().isEmpty()) {
            res += tab + key + " : " + val.toString() + "\n";
        }
    }

    return res;
}

void SmoozikXml::cleanError() {
    _error = SmoozikManager::NoError;
    _errorMsg = QString();
}
