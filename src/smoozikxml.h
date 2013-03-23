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
class SmoozikXml : public QObject
{
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
    SMOOZIKLIB_EXPORT explicit SmoozikXml(QObject *parent = 0);
    /**
     * @brief Constructs a SmoozikXml and parses the reply.
     * @sa parse()
     */
    SMOOZIKLIB_EXPORT explicit SmoozikXml(QNetworkReply *reply, QObject *parent = 0);

    inline SmoozikManager::Error error() const {
        return _error;
    } /**< @see #error */

    inline QString errorMsg() const {
        return _errorMsg;
    } /**< @see #errorMsg */

    /**
     * @brief Parses a QDomElement data to fill SmoozikXml QMap.
     * @param dataElement a QDomElement
     */
    SMOOZIKLIB_EXPORT void parse(const QDomElement &dataElement);

    /**
     * @brief Parses response from Smoozik Server.
     *
     * The reply is deleted during the process.
     * @param reply Response from Smoozik Server
     * @retval true if parsing succeeded. Data is accessible with operator [].
     * @retval false if parsing failed. Error is accessible with error().
     */
    SMOOZIKLIB_EXPORT bool parse(QNetworkReply *reply);

    /**
     * @brief Parses a Dom element and returns it in a QVariant.
     *
     * This function is used recursively by parse().
     */
    SMOOZIKLIB_EXPORT static QVariant parseElement(const QDomElement &element);

    /**
     * @brief Returns the element at @i key of #_parsed if _parsed is a QMap.
     *
     * This element might either be a QString (accessible through QVariant::toString()),
     * a QList (accessible through QVariant::toList())
     * or a QMap (accessible through QVariant::toMap()).
     */
    SMOOZIKLIB_EXPORT QVariant operator[] (const QString &key) const;

    /**
     * @brief Returns the element at index position @i i of #_parsed if _parsed is a QList.
     *
     * This element might either be a QString (accessible through QVariant::toString()),
     * a QList (accessible through QVariant::toList())
     * or a QMap (accessible through QVariant::toMap()).
     */
    SMOOZIKLIB_EXPORT QVariant operator[] (const int i) const;

    /**
     * @brief if _parsed is a QString, return this string; else returns an empty string.
     */
    inline QString parsedString() const {
        return _parsed.toString();
    }

    /**
     * @brief Returns a structured string of the parsed xml to print.
     * @return A structured string
     */
    inline QString print() const {
        return printVariant(_parsed, 0);
    }

    /**
     * @brief Returns a structured string of variant.
     *
     * This function is used recursively by print().
     * @param variant The QVariant to print (either a QString, a QList or a QMap).
     * @param indentCount Indentation
     * @return A structured string
     */
    SMOOZIKLIB_EXPORT static QString printVariant(const QVariant &variant, const int indentCount = 0);

private:
    /**
     * @brief This property holds the QVariant containing the parsed xml.
     */
    QVariant _parsed;
    SmoozikManager::Error _error; /**< @see #error */
    QString _errorMsg; /**< @see #errorMsg */

    /**
     * @brief Cleans error and error message.
     */
    void cleanError();
};

#endif // SMOOZIKXML_H
