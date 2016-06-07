/*
 *  Copyright (C) 2016 Daniel Cárdenes
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see http://www.gnu.org/licenses/.
 *
 *  Version: 1.0
 *  Design and Implementation:  Daniel Cárdenes
 */

#ifndef PAYLOADMANAGER_H
#define PAYLOADMANAGER_H

#include <QObject>
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QDateTime>
#include <QSysInfo>
#include <QApplication>
#include <QTextStream>
#include <QFileSystemModel>

//Number of bytes transmitted in a SIGFOX message by SCAN-device
#define PAYLOAD_SIZE    11

class PayloadManager : public QObject
{
    Q_OBJECT
    Q_ENUMS(returnCodes)
public:
    enum returnCodes {NO_ERROR, EMPTY_FILE, FORMAT_ERROR, JSON_PARSE_ERROR, JSON_OBJECT_ERROR, UNABLE_OPEN_IN_FILE,
                      UNABLE_OPEN_OUT_FILE};
    explicit PayloadManager(QObject *parent = 0);
    int importRxRawDataFile(QString fileToImport, QString destinationDir);
    int importTxRawDataFile(QString fileToImport, QString destinationDir);
    bool decodeRawJsonData(QJsonObject inputObject, QJsonObject *outputObject);
    QString fromRawGeopositionToDegrees(QString rawGeo);
    int processData(QString rxFileName, QString rxDir, QModelIndexList txFileNameList, QFileSystemModel *processTxDataFilesModel, QString txDir, QString outDir);
    //this method generates a ready geo feature once array has been processed against object parameter.
    QJsonObject getFeature(QJsonObject txJsonObject, QJsonArray rxJsonArray);
private:
    //this struct stores the payload sent by device once decoded
    typedef struct{
        QString txID;
        QString power;
        QString latitude;
        QString longitude;
    } decodedPayload;

    decodedPayload getDecodedPayload(QString codedPayload);

signals:

public slots:
};

#endif // PAYLOADMANAGER_H
