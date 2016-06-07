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

#include "dialogdownloadfromdevice.h"
#include "ui_dialogdownloadfromdevice.h"

#define RAW_DIR "raw"


DialogDownloadFromDevice::DialogDownloadFromDevice(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogDownloadFromDevice)
{
    ui->setupUi(this);

    //initialize reception buffer
    deviceRawData="";

    //create serial port
    serialPort=new QSerialPort(this);

    //connect signals
    connect(serialPort,SIGNAL(readyRead()), this, SLOT(readData()));

    //get a list with available ports
    QList<QSerialPortInfo> availablePorts=QSerialPortInfo::availablePorts();
    //fill combo box with available ports
    foreach (const QSerialPortInfo &port, availablePorts)
            this->ui->serialPortscomboBox->addItem(port.portName());
}

DialogDownloadFromDevice::~DialogDownloadFromDevice()
{
    delete ui;
}

//Read available serial port data
void DialogDownloadFromDevice::readData()
{
    QByteArray receivedData = serialPort->readAll();
    QString tempString(receivedData);

   deviceRawData.append(tempString);

    if (deviceRawData.endsWith("EOF\r\n"))
    {
        //Reception completed

        //save to file
        QString currentDir=QApplication::applicationDirPath();

        QSettings settings(currentDir+"/config.ini",QSettings::IniFormat);
        QString extension=settings.value("Extensions/txRawData").toString();

        QString timeStamp=QDateTime::currentDateTime().toString("dd-MM-yyyy-hhmmss-");
        QString fileName(currentDir+"/"+RAW_DIR+"/"+timeStamp+"device-raw-data."+extension);
        QFile outputFile(fileName);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            ui->statuslabel->setText("[ERROR] Unable to save file");
            deviceRawData="";

        } else
        {

            QTextStream outputStream(&outputFile);
            outputStream << deviceRawData;

            outputFile.close();
            ui->statuslabel->setText("[INFO] Data succesfully downloaded");
            deviceRawData="";
        }

    }

}

//Download data from SCAN-device
void DialogDownloadFromDevice::on_downloadpushButton_clicked()
{
    if (serialPort->isOpen())
    {
        ui->statuslabel->setText("[INFO] Downloading data");
        //prepare command to be sent to device

        QByteArray command("$GD$");
        serialPort->write(command);
    } else
    {
        ui->statuslabel->setText("[ERROR] Serial port is closed. Open it first");
    }
}

void DialogDownloadFromDevice::on_openPortpushButton_clicked()
{
    //try to open port
    ui->statuslabel->setText("[INFO] Opening serial port...");

    serialPort->setPortName(ui->serialPortscomboBox->currentText());
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);

    //close port if it is open
    if (serialPort->isOpen()) serialPort->close();

    if (serialPort->open(QIODevice::ReadWrite))
    {
        ui->statuslabel->setText("[INFO] Opened sucessfuly. Press Remote on device");
    } else
    {
        ui->statuslabel->setText("[ERROR] Can not open serial port. Check permission");
    }


}
