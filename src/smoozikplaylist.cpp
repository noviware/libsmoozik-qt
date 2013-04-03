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

#include "smoozikplaylist.h"
#include "smoozikxml.h"

SmoozikPlaylist::SmoozikPlaylist(QObject *parent) :
    QObject(parent)
{
}

SmoozikPlaylist::SmoozikPlaylist(const QDomDocument &doc, QObject *parent) :
    QObject(parent)
{
    addTracks(doc);
}

SmoozikPlaylist::SmoozikPlaylist(const QVariantList &list, QObject *parent) :
    QObject(parent)
{
    addTracks(list);
}

SmoozikPlaylist::~SmoozikPlaylist()
{

}

void SmoozikPlaylist::addTrack(SmoozikTrack *track)
{
    if (!contains(track->localId())) {
        _list.append(track);
    }
}

void SmoozikPlaylist::addTracks(const QDomDocument &doc)
{
    return addTracks(SmoozikXml::parseElement(doc.firstChildElement()).toList());
}

void SmoozikPlaylist::addTracks(const QVariantList &list)
{

    foreach(QVariant variant, list) {
        if (!variant.toMap().isEmpty()) {
            if (variant.toMap().contains("track")) {
                addTrack(new SmoozikTrack(variant.toMap()["track"].toMap(), this));
            }
        }
    }
}

bool SmoozikPlaylist::contains(const QString &localId) const
{

    foreach(SmoozikTrack *track, _list) {
        if (track->localId() == localId) {
            return true;
        }
    }
    return false;
}

int SmoozikPlaylist::indexOf(const QString &localId) const
{
    int listCount = count();
    for (int i = 0; i < listCount; i++) {
        if (value(i)->localId() == localId) {
            return i;
        }
    }
    return -1;
}
