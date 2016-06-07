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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSettings>
#include <QFile>
#include <QMessageBox>
#include <QDir>
#include <QDebug>
#include <QLabel>
#include <QFileSystemModel>
#include "payloadmanager.h"
#include "dialogviewfile.h"
#include "dialogrenamefile.h"
#include "dialogdownloadfromserver.h"
#include "dialogdownloadfromdevice.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_ENUMS(statusLevel)

public:
    enum statusLevel{NO_ERROR,WARNING,ERROR};
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_rxRawDatafilesImportpushButton_clicked();

    void on_txRawDatafilesImportpushButton_clicked();

    void on_ProcesspushButton_clicked();

    void on_clearMappushButton_clicked();

    void on_loadServiceMappushButton_clicked();

    void on_txRawDataFileViewpushButton_clicked();

    void on_txImportedFilesViewpushButton_clicked();

    void on_rxRawDataFileViewpushButton_clicked();

    void on_rxImportedFilesViewpushButton_clicked();

    void on_ProcessViewpushButton_clicked();

    void on_txRawDataFilesDeletepushButton_clicked();

    void on_tximportedFilesDeletepushButton_clicked();

    void on_rxRawDataFilesDeletepushButton_clicked();

    void on_importedFilesDeletepushButton_clicked();

    void on_ProcessDeletepushButton_clicked();

    void on_txRawDataFileRenamepushButton_clicked();

    void on_txImportedFileRenamepushButton_clicked();

    void on_rxRawDataFileRenamepushButton_clicked();

    void on_rxImportedFileRenamepushButton_clicked();

    void on_processRenamepushButton_clicked();

    void on_getFromServerpushButton_clicked();

    void updateLoadProgress(int progress);

    void updateLoadFinished(bool ok);

    void showStatistics(QString serviceFileName);

    void printErrorOnStatistics();

    void clearStatistics();

    void on_getFromDevicepushButton_clicked();

private:
    Ui::MainWindow *ui;

private:
    void loadStartupMap();
    QLabel *ledStatus;
    void writeStatus(QString status,int level);
    QString currentDir;

    //Data Models
    QFileSystemModel *txRawDataFilesModel;
    QFileSystemModel *txImportedFilesModel;
    QFileSystemModel *rxRawDataFilesModel;
    QFileSystemModel *rxImportedFilesModel;
    QFileSystemModel *processTxDataFilesModel;
    QFileSystemModel *processRxDataFilesModel;
    QFileSystemModel *processProcessedDataFilesModel;

    PayloadManager myPayloadManager;
};

#endif // MAINWINDOW_H
