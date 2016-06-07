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

#ifndef DIALOGDOWNLOADFROMSERVER_H
#define DIALOGDOWNLOADFROMSERVER_H

#include <QDialog>
#include <QSettings>
#include <QFile>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

namespace Ui {
class DialogDownloadFromServer;
}

class DialogDownloadFromServer : public QDialog
{
    Q_OBJECT

public:
    explicit DialogDownloadFromServer(QWidget *parent = 0);
    ~DialogDownloadFromServer();

private slots:
    void on_downloadServerpushButton_clicked();
    void replyFinished();
    void updateProgressBar(qint64 receivedBytes, qint64 totalBytes);
    void downloadError(QNetworkReply::NetworkError errorCode);

private:
    Ui::DialogDownloadFromServer *ui;

     QNetworkAccessManager *manager;
     QNetworkReply *reply;
};

#endif // DIALOGDOWNLOADFROMSERVER_H
