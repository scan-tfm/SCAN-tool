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

#ifndef DIALOGDOWNLOADFROMDEVICE_H
#define DIALOGDOWNLOADFROMDEVICE_H

#include <QDialog>
#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>

namespace Ui {
class DialogDownloadFromDevice;
}

class DialogDownloadFromDevice : public QDialog
{
    Q_OBJECT

private slots:
    void readData();

    void on_downloadpushButton_clicked();

    void on_openPortpushButton_clicked();

public:
    explicit DialogDownloadFromDevice(QWidget *parent = 0);
    ~DialogDownloadFromDevice();

private:
    Ui::DialogDownloadFromDevice *ui;

    QSerialPort *serialPort;
    QByteArray receivedData;
    QString deviceRawData;
};

#endif // DIALOGDOWNLOADFROMDEVICE_H
