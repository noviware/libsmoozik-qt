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

#include "testsmoozikmanager.h"
#include "global.h"

#include <QDomElement>

void TestSmoozikManager::login() {
    SmoozikManager *smoozikManager = new SmoozikManager(APIKEY, this, SECRET, SmoozikManager::XML, true);
    QNetworkReply *reply = smoozikManager->login(USERNAME, PASSWORD);

    qDebug() << reply->url().toString();
    //Teste s'il y a eu des erreurs durant la requête
    if (reply->error()) {
        qDebug() << reply->readAll();
        qDebug() << QString("Error during network request: %1").arg(reply->errorString());
        QFAIL("Error during network request");
    }

    //Get xml from reply
    QString xmlReply = reply->readAll();
    qDebug() << xmlReply;

    QDomDocument xml;
    QString errorMsg;
    int errorLine;
    int errorColumn;
    if (!xml.setContent(xmlReply, &errorMsg, &errorLine, &errorColumn)) {
        qDebug() << ("Impossible to parse xml, Error: " + errorMsg +
                ", Line: " + QString::number(errorLine) +
                ", Colonne: " + QString::number(errorColumn));
        QFAIL("Impossible to parse time xml");
    }

    QDomElement smoozikElement = xml.firstChildElement("smoozik");
    if (smoozikElement.isNull()) {
        QFAIL("No smoozik element in reply");
    }

    QDomElement statusElement = smoozikElement.firstChildElement("status");
    if (statusElement.isNull()) {
        QFAIL("No status element in reply");
    }

    if (statusElement.text() != "ok") {
        QFAIL("Status is not ok");
    }

    QDomElement dataElement = smoozikElement.firstChildElement("data");
    if (dataElement.isNull()) {
        QFAIL("No data element in reply");
    }

    QDomElement sessionKeyElement = dataElement.firstChildElement("sessionKey");
    if (sessionKeyElement.isNull()) {
        QFAIL("No sessionKey element in reply");
    }

    QString sessionKey = sessionKeyElement.text();
    if (sessionKey == "") {
        QFAIL("sessionKey is empty");
    }

    //Delete reply
    reply->manager()->deleteLater();
}

QTEST_MAIN(TestSmoozikManager)
