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

#ifndef SIMPLEHTTPSERVER_H
#define SIMPLEHTTPSERVER_H

#include <QTcpServer>
#include <QTcpSocket>

/**
 * @brief The SimpleHttpServer class is a simple server used for tests of Smoozik lib.
 *
 * It only replies #response to any request.
 * It is inspired by Qt Simple Http Server example
 */
class SimpleHttpServer : public QTcpServer {
    Q_OBJECT
    /**
     * @brief This property holds the response the server will send to any request it receives.
     * @af response(), setResponse()
     * @pm _response
     */
    Q_PROPERTY(QString response READ response WRITE setResponse)
public:
    explicit SimpleHttpServer(quint16 port, QObject* parent = 0);

    void incomingConnection(int socket);

    inline QString response() const {
        return _response;
    } /**< @see #response */

    inline void setResponse(const QString &response) {
        _response = response;
    } /**< @see #response */

private:
    QString _response; /**< @see #response */

private slots:
    void readClient();
    void discardClient();
};

#endif // SIMPLEHTTPSERVER_H
