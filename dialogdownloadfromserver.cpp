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

#include "dialogdownloadfromserver.h"
#include "ui_dialogdownloadfromserver.h"

#define RAW_DIR "raw"

DialogDownloadFromServer::DialogDownloadFromServer(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDownloadFromServer)
{
    ui->setupUi(this);
   QString currentDir=QApplication::applicationDirPath();

   QSettings settings(currentDir+"/config.ini",QSettings::IniFormat);
   QString server=settings.value("Application/server").toString();

   //Load a list with all devices included in config.ini

   QMap<QString,QString> deviceMap;
   settings.beginGroup("Devices");
   QStringList keys = settings.childKeys();

   foreach (QString key, keys)
   {
       deviceMap[key]=settings.value(key).toString();
   }
   QStringList devices=deviceMap.values();

   settings.endGroup();

   this->ui->serverlineEdit->setText(server);
   this->ui->statusServerlabel->setText("");
   this->ui->devicecomboBox->addItems(devices);

}

DialogDownloadFromServer::~DialogDownloadFromServer()
{
    delete ui;
}

//Download file from SCAN-server
void DialogDownloadFromServer::on_downloadServerpushButton_clicked()
{
    QString currentDir=QApplication::applicationDirPath();

    QSettings settings(currentDir+"/config.ini",QSettings::IniFormat);
    QString extension=settings.value("Extensions/rxRawData").toString();

    QString url(ui->serverlineEdit->text()+"/"+ui->devicecomboBox->currentText()+"."+extension);
    ui->statusServerlabel->setText("Downloading file...");

    manager=new QNetworkAccessManager(this);

    reply=manager->get(QNetworkRequest(QUrl(url)));

    //connect signals
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(updateProgressBar(qint64,qint64)));

    connect(reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(downloadError(QNetworkReply::NetworkError)));

    connect(reply, SIGNAL(finished()),this, SLOT(replyFinished()));

}


//Save downloaded data
void DialogDownloadFromServer::replyFinished()
{
    QString currentDir=QApplication::applicationDirPath();

    QSettings settings(currentDir+"/config.ini",QSettings::IniFormat);
    QString extension=settings.value("Extensions/rxRawData").toString();
    QString device(ui->devicecomboBox->currentText());
    QByteArray downloadedData;
    downloadedData=reply->readAll();
    reply->deleteLater();

    if (!reply->error())
    {

        //save outputFile
        QString timeStamp=QDateTime::currentDateTime().toString("dd-MM-yyyy-hhmmss-");
        QString fileName(currentDir+"/"+RAW_DIR+"/"+timeStamp+"device-"+device+"."+extension);
        QFile outputFile(fileName);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            ui->statusServerlabel->setText("Unable to save file");

        } else
        {

            outputFile.write(downloadedData);
            outputFile.close();
            ui->statusServerlabel->setText("Download finished");
        }
    }


}

//Updte download progress bar
void DialogDownloadFromServer::updateProgressBar(qint64 receivedBytes, qint64 totalBytes)
{
    ui->serverprogressBar->setMaximum(totalBytes);
    ui->serverprogressBar->setValue(receivedBytes);

}

//Write download error
void DialogDownloadFromServer::downloadError(QNetworkReply::NetworkError errorCode)
{

    ui->statusServerlabel->setText("Download error!");
    reply->deleteLater();
}
