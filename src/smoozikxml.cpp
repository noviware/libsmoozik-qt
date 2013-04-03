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
    QObject(parent)
{
    cleanError();
}

SmoozikXml::SmoozikXml(QNetworkReply *reply, QObject *parent) :
    QObject(parent)
{
    cleanError();
    parse(reply);
}

SmoozikXml::~SmoozikXml()
{

}

void SmoozikXml::parse(const QDomElement &dataElement)
{
    cleanError();

    _parsed = parseElement(dataElement);
}

bool SmoozikXml::parse(QNetworkReply *reply)
{
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

QVariant SmoozikXml::operator [](const QString &key) const
{
    if (!_parsed.toMap().isEmpty()) {
        return _parsed.toMap()[key];
    }
    return QVariant();
}

QVariant SmoozikXml::operator [](const int i) const
{
    if (!_parsed.toList().isEmpty()) {
        return _parsed.toList().value(i);
    }
    return QVariant();
}

QVariant SmoozikXml::parseElement(const QDomElement &element)
{
    QVariant variant;
    // Case when element is text
    QDomText t = element.firstChild().toText();
    if (!t.isNull()) {
        variant.setValue(t.data());
        return variant;
    }

    QDomElement fe = element.firstChildElement();
    //Case when element is an array (there is another element with same tag in list or item name is singular of parent name
    if (!fe.nextSiblingElement(fe.tagName()).isNull() || element.tagName() == fe.tagName() + "s") {
        QVariantList list;
        for (QDomElement e = element.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
            QVariantMap map;
            QVariant child = parseElement(e);
            map[e.tagName()] = child;
            list.append(map);
        }
        variant.setValue(list);
        return variant;
    }

    //Case when element is not an array
    QVariantMap map;
    for (QDomElement e = element.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        QVariant child = parseElement(e);
        map[e.tagName()] = child;
    }
    variant.setValue(map);
    return variant;
}

QString SmoozikXml::printVariant(const QVariant &variant, const int indentCount)
{
    QString res;
    QString tab = "    ";
    QString indent;
    for (int i = 0; i < indentCount; i++) {
        indent += tab;
    }

    if (!variant.toString().isEmpty()) {
        res = variant.toString() + "\n";
    } else if (!variant.toList().isEmpty()) {
        res += "{\n";

        foreach(QVariant child, variant.toList()) {
            res += indent + tab + printVariant(child, indentCount + 1);
        }
        res += indent + "}\n";
    } else if (!variant.toMap().isEmpty()) {
        res += "{\n";

        foreach(QString key, variant.toMap().keys()) {
            res += indent + tab + key + " : " + printVariant(variant.toMap()[key], indentCount + 1);
        }
        res += indent + "}\n";
    } else {
        res = "\n";
    }

    return res;
}

void SmoozikXml::cleanError()
{
    _error = SmoozikManager::NoError;
    _errorMsg = QString();
}
