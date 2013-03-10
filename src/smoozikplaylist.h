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

#ifndef SMOOZIKPLAYLIST_H
#define SMOOZIKPLAYLIST_H

#include <QObject>
#include <QVariantList>
#include <QDomDocument>

#include "global.h"
#include "smooziktrack.h"

/**
 * @brief The SmoozikPlaylist class represents a collection of track
 */
class SmoozikPlaylist : public QObject {
    Q_OBJECT

public:
    SMOOZIKLIB_EXPORT explicit SmoozikPlaylist(QObject *parent = 0);
    /**
     * @brief Constructs a SmoozikPlaylist and fills it with data from DomDocument @i doc.
     * @param doc DomDocument containing a list of <track> elements
     * @param parent
     */
    SMOOZIKLIB_EXPORT explicit SmoozikPlaylist(const QDomDocument &doc, QObject *parent = 0);
    /**
     * @brief Constructs a SmoozikPlaylist and fills it with data from QVariantList @i list.
     * @param list QVariantList containing a list of track elements parsed using SmoozikXml
     * @param parent
     */
    SMOOZIKLIB_EXPORT explicit SmoozikPlaylist(const QVariantList &list, QObject *parent = 0);

    /**
     * @brief Adds a track to the playlist.
     *
     * If the playlist already has a track with this localId, the track is not added.
     * @param track
     */
    SMOOZIKLIB_EXPORT void addTrack(SmoozikTrack *track);

    /**
     * @brief Adds a track to the playlist.
     *
     * If the playlist already has a track with this localId, the track is not added.
     * @param localId Local unique Id of the track
     * @param name Name of the track
     * @param artist Artist of the track
     * @param album Album name of the track
     * @param duration Duration of the track
     */
    inline void addTrack(QString localId, QString name, QString artist = QString(), QString album = QString(), uint duration = 0) {
        addTrack(new SmoozikTrack(localId, name, this, artist, album, duration));
    }

    /**
     * @brief Adds tracks from DomDocument @i doc to the playlist.
     * @param doc DomDocument containing a list of <track> elements
     */
    SMOOZIKLIB_EXPORT void addTracks(const QDomDocument &doc);

    /**
     * @brief Adds tracks from QVariantList @i doc to the playlist.
     * @param list QVariantList containing a list of track elements parsed using SmoozikXml
     */
    SMOOZIKLIB_EXPORT void addTracks(const QVariantList &list);

    /**
     * @brief Returns true if the playlist contains a track with @i localId; otherwise returns false.
     */
    SMOOZIKLIB_EXPORT bool contains(const QString &localId) const;

    /**
     * @brief Returns the index position of the first occurrence of track with @i localId in the playlist, searching forward from index position from. Returns -1 if no item matched.
     */
    SMOOZIKLIB_EXPORT int indexOf(const QString &localId) const;

    /**
     * @brief Clear playlist and deletes all playlist tracks.
     */
    inline void deleteTracks() {
        qDeleteAll(_list);
    }

    /**
     * @name QList methods
     */
    //@{

    inline void clear() {
        return _list.clear();
    } /**< Aggregation of QList equivalent method */

    inline int count() const {
        return _list.count();
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *first() {
        return _list.first();
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *first() const {
        return _list.first();
    } /**< Aggregation of QList equivalent method */

    inline bool isEmpty() const {
        return _list.isEmpty();
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *last() {
        return _list.last();
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *last() const {
        return _list.last();
    } /**< Aggregation of QList equivalent method */

    inline void removeAt(int i) {
        return _list.removeAt(i);
    } /**< Aggregation of QList equivalent method */

    inline void removeFirst() {
        return _list.removeFirst();
    } /**< Aggregation of QList equivalent method */

    inline void removeLast() {
        return _list.removeLast();
    } /**< Aggregation of QList equivalent method */

    inline int size() const {
        return _list.size();
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *takeAt(int i) {
        return _list.takeAt(i);
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *takeFirst() {
        return _list.takeFirst();
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *takeLast() {
        return _list.takeLast();
    } /**< Aggregation of QList equivalent method */

    inline SmoozikTrack *value(int i) const {
        return _list.value(i);
    } /**< Aggregation of QList equivalent method */
    //@}

private:
    /**
     * @brief This property holds the QList containing the SmoozikTrack objects.
     */
    QList<SmoozikTrack *> _list;
};

#endif // SMOOZIKPLAYLIST_H
