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

#ifndef SMOOZIKTRACK_H
#define SMOOZIKTRACK_H

#include <QObject>
#include <QVariantMap>
#include <QDomDocument>

#include "global.h"

/**
 * @brief The SmoozikTrack class represents a track.
 */
class SmoozikTrack : public QObject
{
    /**
     * @brief This property holds the local unique Id of the track.
     *
     * This Id should allow you to identify any track in your local database.
     * This could be a path or an id given by a software inner database.
     * @af localId()
     * @pm _localId
     */
    Q_PROPERTY(QString localId READ localId)
    /**
     * @brief This property holds the name of the track.
     * @af name()
     * @pm _name
     */
    Q_PROPERTY(QString name READ name)
    /**
     * @brief This property holds the artist of the track.
     * @af artist()
     * @pm _artist
     */
    Q_PROPERTY(QString artist READ artist)
    /**
     * @brief This property holds the album name of the track.
     * @af album()
     * @pm _album
     */
    Q_PROPERTY(QString album READ album)
    /**
     * @brief This property holds the duration of the track.
     * @af duration()
     * @pm _duration
     */
    Q_PROPERTY(uint duration READ duration)
    Q_OBJECT

public:
    SMOOZIKLIB_EXPORT explicit SmoozikTrack(const QString &localId, const QString &name, QObject *parent = 0, const QString &artist = QString(), const QString &album = QString(), uint duration = 0);
    /**
     * @brief Constructs a SmoozikTrack and fills its properties with data from DomDocument @i doc.
     * @param doc DomDocument containing a list of track properties
     * @param parent
     * @return
     */
    SMOOZIKLIB_EXPORT explicit SmoozikTrack(const QDomDocument &doc, QObject *parent = 0);
    /**
     * @brief Constructs a SmoozikTrack and fills its properties with data from QVariantMap @i map.
     * @param map QVariantMap containing a list of track properties
     * @param parent
     * @return
     */
    SMOOZIKLIB_EXPORT explicit SmoozikTrack(const QVariantMap &map, QObject *parent = 0);

    SMOOZIKLIB_EXPORT ~SmoozikTrack();

    inline QString localId() const {
        return _localId;
    } /**< see #localId */

    inline QString name() const {
        return _name;
    } /**< see #name */

    inline QString artist() const {
        return _artist;
    } /**< see #artist */

    inline QString album() const {
        return _album;
    } /**< see #album */

    inline uint duration() const {
        return _duration;
    } /**< see #duration */

private:
    QString _localId; /**< see #localId */
    QString _name; /**< see #name */
    QString _artist; /**< see #artist */
    QString _album; /**< see #album */
    uint _duration; /**< see #duration */
    /**
     * @brief Sets track properties with data from QVariantMap @i map.
     * @param map QVariantMap containing a list of track properties
     */
    void setPropertiesFromMap(const QVariantMap &map);
};

#endif // SMOOZIKTRACK_H
