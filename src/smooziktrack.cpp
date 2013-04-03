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

#include "smooziktrack.h"
#include "smoozikxml.h"

SmoozikTrack::SmoozikTrack(const QString &localId, const QString &name, QObject *parent, const QString &artist, const QString &album, uint duration) :
    QObject(parent)
{
    _localId = localId;
    _name = name;
    _artist = artist;
    _album = album;
    _duration = duration;
}

SmoozikTrack::SmoozikTrack(const QDomDocument &doc, QObject *parent) :
    QObject(parent)
{
    setPropertiesFromMap(SmoozikXml::parseElement(doc.firstChildElement()).toMap());
}

SmoozikTrack::SmoozikTrack(const QVariantMap &map, QObject *parent) :
    QObject(parent)
{
    setPropertiesFromMap(map);
}

SmoozikTrack::~SmoozikTrack()
{

}

void SmoozikTrack::setPropertiesFromMap(const QVariantMap &map)
{
    (map.contains("localId")) ? _localId = map["localId"].toString() : _localId = QString();
    (map.contains("name")) ? _name = map["name"].toString() : _name = QString();
    (map.contains("artist")) ? _artist = map["artist"].toString() : _artist = QString();
    (map.contains("album")) ? _album = map["album"].toString() : _album = QString();
    (map.contains("duration")) ? _duration = map["duration"].toString().toInt() : _duration = 0;
}
