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

#ifndef SMOOZIKXML_H
#define SMOOZIKXML_H

#include <QObject>
#include <QDomDocument>

#include "smoozikmanager.h"

/**
 * @brief The SmoozikXml class provides with functions to parse XML response from Smoozik webserver
 */
class SmoozikXml : public QObject, public QVariantMap {
    /**
     * @brief This property holds the error encountered in last parse() call.
     *
     * If no error where found in last parse() call or if parse() has not ever been called, returns SmoozikManager::NoError
     * @af error()
     * @pm _error
     */
    Q_PROPERTY(SmoozikManager::Error error READ error)
    /**
     * @brief This property holds the error message of the error encountered in last parse() call.
     *
     * If no error where found in last parse() call or if parse() has not ever been called, returns a null String.
     * @af errorMsg()
     * @pm _errorMsg
     */
    Q_PROPERTY(QString errorMsg READ errorMsg)
    Q_OBJECT
public:
    explicit SmoozikXml(QObject *parent = 0);

    inline SmoozikManager::Error error() const {
        return _error;
    } /**< @see #error */

    inline QString errorMsg() const {
        return _errorMsg;
    } /**< @see #errorMsg */

    /**
     * @brief Parses a QDomElement data.
     * @param data a QDomElement
     * @retval true if parsing succeeded. Data is accessible with operator [].
     * @retval false if parsing failed. Error is accessible with error().
     */
    bool parse(const QDomElement &dataElement);

    /**
     * @brief Parses response from Smoozik Server.
     *
     * The reply is deleted during the process.
     * @param reply Response from Smoozik Server
     * @retval true if parsing succeeded. Data is accessible with operator [].
     * @retval false if parsing failed. Error is accessible with error().
     */
    bool parse(QNetworkReply *reply);

    /**
     * @brief Returns a structured string of the parsed xml to print.
     * @return A structured string
     */
    QString print();

private:
    SmoozikManager::Error _error; /**< @see #error */
    QString _errorMsg; /**< @see #errorMsg */
    /**
     * @brief Parses a Dom element and add it to the QMap pointed by #_parsingPointer.
     *
     * This function is used recursively by parse().
     * @param map The map to which the element should be added
     * @param element DomElement to parse
     * @retval true if parsing succeed.
     */
    bool parseElement(QVariantMap *map, const QDomElement &element);

    /**
     * @brief Returns a structured string of #map.
     *
     * This function is used recursively by print().
     * @param map The QMap to print.
     * @param indent Indentation
     * @return A structured string
     */
    QString printMap(const QVariantMap *map, const int indent = 0);
    /**
     * @brief Cleans error and error message.
     */
    void cleanError();

};

#endif // SMOOZIKXML_H
