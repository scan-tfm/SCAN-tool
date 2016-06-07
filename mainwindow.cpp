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

#include "mainwindow.h"
#include "ui_mainwindow.h"

//define directories
#define RAW_DIR "raw"
#define RX_IMPORTED_DIR "rx"
#define TX_IMPORTED_DIR "tx"
#define OUT_DIR "out"
#define TEMP_DIR "temp"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //setup legend
    ui->blackLabel->setStyleSheet("QLabel { background-color : black; color : black; }");
    ui->redLabel->setStyleSheet("QLabel { background-color : red; color : black; }");
    ui->yellowLabel->setStyleSheet("QLabel { background-color : yellow; color : black; }");
    ui->greenLabel->setStyleSheet("QLabel { background-color : green; color : black; }");

    //Crete directories if they do not exist
    currentDir=QApplication::applicationDirPath();
    if (!QDir(currentDir+"/raw").exists())
    {
        QDir().mkdir(currentDir+"/raw");
    }

    if (!QDir(currentDir+"/tx").exists())
    {
        QDir().mkdir(currentDir+"/tx");
    }

    if (!QDir(currentDir+"/rx").exists())
    {
        QDir().mkdir(currentDir+"/rx");
    }

    if (!QDir(currentDir+"/out").exists())
    {
        QDir().mkdir(currentDir+"/out");
    }

    if (!QDir(currentDir+"/temp").exists())
    {
        QDir().mkdir(currentDir+"/temp");
    }


    //Load config.ini file. If it does not exist, create it
    QString configFileName=currentDir+"/config.ini";
    QFile configFile(configFileName);

    if (!configFile.exists())
    {
        //if .ini does not exist, then load default parameter and creat it
        QSettings settings(configFileName,QSettings::IniFormat);
        settings.beginGroup("Application");
            settings.setValue("lastLat", "27.971");
            settings.setValue("lastLon","-15.594");
            settings.setValue("lastZoom","10");
            settings.setValue("server","http://scan-server-mydomain.com");
        settings.endGroup();

        settings.beginGroup("Devices");
            settings.setValue("device1", "74545");
        settings.endGroup();

        settings.beginGroup("Extensions");
            settings.setValue("rxRawData", "rawjson");            
            settings.setValue("rxImportedData","json");
            settings.setValue("txRawData", "rawdevice");
            settings.setValue("txImportedData","json");
            settings.setValue("processedData","geojson");

        settings.endGroup();

    }


    QSettings settings(configFileName,QSettings::IniFormat);

    //Create Models and Views for processing directories
    ////Device files Tab
    txRawDataFilesModel=new QFileSystemModel(this);
    //Filter Dir by extension
    QStringList filterBy;
    filterBy << "*."+ settings.value("Extensions/txRawData").toString();
    txRawDataFilesModel->setNameFilters(filterBy);
    txRawDataFilesModel->setNameFilterDisables(false);
    //Link model with view
    ui->txRawDataFileslistView->setModel(txRawDataFilesModel);
    ui->txRawDataFileslistView->setRootIndex(txRawDataFilesModel->setRootPath(currentDir+"/"+RAW_DIR));
    ui->txRawDataFileslistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    txImportedFilesModel=new QFileSystemModel(this);
    //Filter Dir by extension
    filterBy.clear();
    filterBy << "*."+ settings.value("Extensions/txImportedData").toString();
    txImportedFilesModel->setNameFilters(filterBy);
    txImportedFilesModel->setNameFilterDisables(false);
    //Link model with view
    ui->txImportedDataFileslistView->setModel(txImportedFilesModel);
    ui->txImportedDataFileslistView->setRootIndex(txImportedFilesModel->setRootPath(currentDir+"/"+TX_IMPORTED_DIR));
    ui->txImportedDataFileslistView->setEditTriggers(QAbstractItemView::NoEditTriggers);


    ////Server files Tab
    rxRawDataFilesModel=new QFileSystemModel(this);
    //Filter Dir by extension
    filterBy << "*."+ settings.value("Extensions/rxRawData").toString();
    rxRawDataFilesModel->setNameFilters(filterBy);
    rxRawDataFilesModel->setNameFilterDisables(false);
    //Link model with view
    ui->rxRawDataFileslistView->setModel(rxRawDataFilesModel);
    ui->rxRawDataFileslistView->setRootIndex(rxRawDataFilesModel->setRootPath(currentDir+"/"+RAW_DIR));
    ui->rxRawDataFileslistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    rxImportedFilesModel=new QFileSystemModel(this);
    //Filter Dir by extension
    filterBy.clear();
    filterBy << "*."+ settings.value("Extensions/rxImportedData").toString();
    rxImportedFilesModel->setNameFilters(filterBy);
    rxImportedFilesModel->setNameFilterDisables(false);
    //Link model with view
    ui->rxImportedDataFileslistView->setModel(rxImportedFilesModel);
    ui->rxImportedDataFileslistView->setRootIndex(rxImportedFilesModel->setRootPath(currentDir+"/"+RX_IMPORTED_DIR));
    ui->rxImportedDataFileslistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ////Process files Tab
    processTxDataFilesModel=new QFileSystemModel(this);
    //Filter Dir by extension
    filterBy.clear();
    filterBy << "*."+ settings.value("Extensions/txImportedData").toString();
    processTxDataFilesModel->setNameFilters(filterBy);
    processTxDataFilesModel->setNameFilterDisables(false);
    //Link model with view
    ui->ProcessTxlistView->setModel(processTxDataFilesModel);
    //Allow multi-file selection
    ui->ProcessTxlistView->setSelectionMode(QAbstractItemView::MultiSelection);
    ui->ProcessTxlistView->setRootIndex(processTxDataFilesModel->setRootPath(currentDir+"/"+TX_IMPORTED_DIR));
    ui->ProcessTxlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    processRxDataFilesModel=new QFileSystemModel(this);
    //Filter Dir by extension
    filterBy.clear();
    filterBy << "*."+ settings.value("Extensions/rxImportedData").toString();
    processRxDataFilesModel->setNameFilters(filterBy);
    processRxDataFilesModel->setNameFilterDisables(false);
    //Link model with view
    ui->ProcessRxlistView->setModel(processRxDataFilesModel);
    ui->ProcessRxlistView->setRootIndex(processRxDataFilesModel->setRootPath(currentDir+"/"+RX_IMPORTED_DIR));
    ui->ProcessRxlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    processProcessedDataFilesModel=new QFileSystemModel(this);
    //Filter Dir by extension
    filterBy.clear();
    filterBy << "*."+ settings.value("Extensions/processedData").toString();
    processProcessedDataFilesModel->setNameFilters(filterBy);
    processProcessedDataFilesModel->setNameFilterDisables(false);
    //Link model with view
    ui->ProcessOutlistView->setModel(processProcessedDataFilesModel);
    ui->ProcessOutlistView->setRootIndex(processProcessedDataFilesModel->setRootPath(currentDir+"/"+OUT_DIR));
    ui->ProcessOutlistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ////Service Map tab
    //Use the same model as processProcessedDtaFilesModel
    //Link model with view
    ui->servicelistView->setModel(processProcessedDataFilesModel);
    ui->servicelistView->setRootIndex(processProcessedDataFilesModel->setRootPath(currentDir+"/"+OUT_DIR));
    ui->servicelistView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    //connect signals
    connect(ui->serviceMapWebView,SIGNAL(loadProgress(int)), this, SLOT(updateLoadProgress(int)));
    connect(ui->serviceMapWebView,SIGNAL(loadFinished(bool)), this, SLOT(updateLoadFinished(bool)));

    loadStartupMap();

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Load start up map. Empty map without service points
void MainWindow::loadStartupMap()
{
    QString html(
                "<!DOCTYPE html>"
                "<html>"
                "<head>"

                "<meta charset=\"utf-8\" />"

                "<style>"
                        "html, body, #map {"
                            "height: 100%;"
                            "margin: 0px;"
                       "}"

                "</style>"

                "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"

                "<link rel=\"stylesheet\" href=\"http://cdn.leafletjs.com/leaflet/v0.7.7/leaflet.css\" />"

                "</head>"
                "<body>"
                "<div id=\"map\"></div>"
                "<script src=\"http://cdn.leafletjs.com/leaflet/v0.7.7/leaflet.js\"></script>"
                "<script>"

                "var map = L.map('map').setView([27.971, -15.594], 10);"
                "L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {attribution: '&copy; <a href=\"http://osm.org/copyright\">OpenStreetMap</a> contributors'}).addTo(map);"






                "</script>"
                "</body>"
                "</html>"
                );


    ui->serviceMapWebView->setHtml(html);

}

//Write a preformatted message in status bar according to level parameter
void MainWindow::writeStatus(QString status, int level)
{

    QString message="";

    switch (level)
    {
    case NO_ERROR:
        message="[INFO] ";
        break;
    case WARNING:
        message="[WARNING] ";
        break;
    case ERROR:
        message="[ERROR] ";
        break;
    }
    ui->statusBar->showMessage(message+status);
}

//Import the selceted file from raw to rx directory.
void MainWindow::on_rxRawDatafilesImportpushButton_clicked()
{
    if (!ui->rxRawDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file to be Imported");
        msgBox.exec();
    } else {
        QString fileToImport=rxRawDataFilesModel->data(ui->rxRawDataFileslistView->currentIndex()).toString();
        switch (myPayloadManager.importRxRawDataFile(currentDir+"/"+RAW_DIR+"/"+fileToImport,RX_IMPORTED_DIR))
        {
        case PayloadManager::NO_ERROR:
            writeStatus("File imported OK",NO_ERROR);
            break;
        case PayloadManager::EMPTY_FILE:
            writeStatus("Empty file. File not imported",WARNING);
            break;
        case PayloadManager::FORMAT_ERROR:
            writeStatus("Format error found on input File. File not imported",ERROR);
            break;
        case PayloadManager::JSON_PARSE_ERROR:
            writeStatus("Error parsing to JSON. File not imported",ERROR);
            break;
        case PayloadManager::JSON_OBJECT_ERROR:
            writeStatus("Error parsing to JSON. Format error found inside an OBJECT. File not imported",ERROR);
            break;
        case PayloadManager::UNABLE_OPEN_IN_FILE:
            writeStatus("Unable to open input file. File not imported",ERROR);
            break;
        case PayloadManager::UNABLE_OPEN_OUT_FILE:
            writeStatus("Unable to open output file. File not imported",ERROR);
            break;
        default:
            writeStatus("File imported OK",NO_ERROR);


        }
    }
}

//Import the selceted file from raw to tx directory.
void MainWindow::on_txRawDatafilesImportpushButton_clicked()
{
    if (!ui->txRawDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file to be Imported");
        msgBox.exec();
    } else {
        QString fileToImport=txRawDataFilesModel->data(ui->txRawDataFileslistView->currentIndex()).toString();
        switch (myPayloadManager.importTxRawDataFile(currentDir+"/"+RAW_DIR+"/"+fileToImport,TX_IMPORTED_DIR))
        {
        case PayloadManager::NO_ERROR:
            writeStatus("File imported OK",NO_ERROR);
            break;
        case PayloadManager::EMPTY_FILE:
            writeStatus("Empty file. File not imported.",WARNING);
            break;
        case PayloadManager::FORMAT_ERROR:
            writeStatus("Format error found on input File. File not imported",ERROR);
            break;
        case PayloadManager::JSON_PARSE_ERROR:
            writeStatus("Error parsing to JSON. File not imported",ERROR);
            break;
        case PayloadManager::JSON_OBJECT_ERROR:
            writeStatus("Error parsing to JSON. Format error found inside an OBJECT. File not imported",ERROR);
            break;
        case PayloadManager::UNABLE_OPEN_IN_FILE:
            writeStatus("Unable to open input file. File not imported",ERROR);
            break;
        case PayloadManager::UNABLE_OPEN_OUT_FILE:
            writeStatus("Unable to open output file. File not imported",ERROR);
            break;
        default:
            writeStatus("File imported OK",NO_ERROR);


        }
    }
}

//Process selected files
void MainWindow::on_ProcesspushButton_clicked()
{
    //Check if files has been selected
    if (!ui->ProcessTxlistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select at least one file from [Transmitted Data]");
        msgBox.exec();
    } else if (!ui->ProcessRxlistView->currentIndex().isValid())
    {
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file from [Received Data]");
        msgBox.exec();
    } else
    {
        QString rxFileName=processRxDataFilesModel->data(ui->ProcessRxlistView->currentIndex()).toString();
        //qDebug()<< rxFileName;
        //Get the list of selected files
        QModelIndexList txFileNameList=ui->ProcessTxlistView->selectionModel()->selectedIndexes();

        switch (myPayloadManager.processData(rxFileName,RX_IMPORTED_DIR,txFileNameList, processTxDataFilesModel,TX_IMPORTED_DIR,OUT_DIR))
        {
        case PayloadManager::NO_ERROR:
            writeStatus("File imported OK",NO_ERROR);
            break;
        case PayloadManager::EMPTY_FILE:
            writeStatus("Empty file. File not imported.",WARNING);
            break;
        case PayloadManager::FORMAT_ERROR:
            writeStatus("Format error found on input File. File not imported",ERROR);
            break;
        case PayloadManager::JSON_PARSE_ERROR:
            writeStatus("Error parsing JSON. Format error, be sure input file is Json Array. File not imported",ERROR);
            break;
        case PayloadManager::JSON_OBJECT_ERROR:
            writeStatus("Error reading JSON. Format error. File not imported",ERROR);
            break;
        case PayloadManager::UNABLE_OPEN_IN_FILE:
            writeStatus("Unable to open input file. File not imported",ERROR);
            break;
        case PayloadManager::UNABLE_OPEN_OUT_FILE:
            writeStatus("Unable to open output file. File not imported",ERROR);
            break;
        default:
            writeStatus("File imported OK",NO_ERROR);


        }

    }


}

//Clear the displayed map and load an empty one
void MainWindow::on_clearMappushButton_clicked()
{
    clearStatistics();
    loadStartupMap();
}

//Load and dispaly the selected geojson file into map
void MainWindow::on_loadServiceMappushButton_clicked()
{
    //Check if files has been selected
    if (!ui->servicelistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a service file to show on map");
        msgBox.exec();
    } else
    {
        QString serviceFileName=processProcessedDataFilesModel->data(ui->servicelistView->currentIndex()).toString();
        //QString outDir=OUT_DIR;
       serviceFileName=currentDir+"/"+OUT_DIR+"/"+serviceFileName;


        // Generate the html code
        QString html(
                    "<!DOCTYPE html>"
                    "<html>"
                    "<head>"

                    "<meta charset=\"utf-8\" />"

                    "<style>"
                            "html, body, #map {"
                                "height: 100%;"
                                "margin: 0px;"
                           "}"

                    "</style>"

                    "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">"

                    "<link rel=\"stylesheet\" href=\"http://cdn.leafletjs.com/leaflet/v0.7.7/leaflet.css\" />"
                    "</head>"
                    "<body>"

                    "<div id=\"map\"></div>"
                    "<script src=\"http://cdn.leafletjs.com/leaflet/v0.7.7/leaflet.js\"></script>"
                    "<script src=\"file://" + currentDir + "/jquery-2.1.1.min.js\"></script>"
                    "<script>"
                    "var map = L.map('map').setView([27.971, -15.594], 10);"
                    "L.tileLayer('http://{s}.tile.osm.org/{z}/{x}/{y}.png', {attribution: '&copy; <a href=\"http://osm.org/copyright\">OpenStreetMap</a> contributors'}).addTo(map);"

                     "$.getJSON(\"file://" +serviceFileName+ "\",function(data){"
                    "var geojsonMarkerOptions1 = {"
                    "radius: 12,"
                    "fillColor: \"#000000\","
                    "color: \"#000\","
                    "weight: 1,"
                    "opacity: 1,"
                    "fillOpacity: 0.8"
                "};"

                    "var geojsonMarkerOptions2 = {"
                        "radius: 12,"
                        "fillColor: \"#ff0000\","
                        "color: \"#000\","
                        "weight: 1,"
                        "opacity: 1,"
                        "fillOpacity: 0.8"
                    "};"

                    "var geojsonMarkerOptions3 = {"
                        "radius: 12,"
                        "fillColor: \"#ffff00\","
                        "color: \"#000\","
                        "weight: 1,"
                        "opacity: 1,"
                        "fillOpacity: 0.8"
                    "};"

                    "var geojsonMarkerOptions4 = {"
                        "radius: 12,"
                        "fillColor: \"#33cc33\","
                        "color: \"#000\","
                        "weight: 1,"
                        "opacity: 1,"
                        "fillOpacity: 0.8"
                    "};"


                     "L.geoJson(data,{"
                         "pointToLayer: function (feature, latlng) {"
                             "var marker;"
                         "var	stations=feature.properties.stations;"
                         "if (stations == 0) {"
                             "marker=L.circleMarker(latlng, geojsonMarkerOptions1);"
                         "} else if (stations == 1) {"
                             "marker=L.circleMarker(latlng, geojsonMarkerOptions2);"
                         "} else if (stations == 2) {"
                             "marker=L.circleMarker(latlng, geojsonMarkerOptions3);"
                         "} else {"
                             "marker=L.circleMarker(latlng, geojsonMarkerOptions4);"
                         "}"

                         "marker.bindPopup(feature.properties.popupContent);"
                         "return marker;"
                         "}"

                     "}).addTo(map);"
                   "});"


                   "</script>"
                   "</body>"
                   "</html>"

                    );

        //ui->serviceMapWebView->setHtml(html,QUrl(":/html"));
        //ui->serviceMapWebView->setHtml(html,QApplication::applicationDirPath());
        QString localPath=currentDir;



        QFile tempFile(localPath+ "/"+TEMP_DIR + "/tempIndex.html");
        if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)){
            writeStatus("Error rendering Service Map. Unable to create temp file", ERROR);
        } else
        {
            QTextStream htmlStream(&tempFile);
            htmlStream<< html;

            tempFile.close();


            ui->serviceMapWebView->load(QUrl::fromLocalFile(currentDir+ "/"+ TEMP_DIR + "/tempIndex.html"));
            ui->serviceMapWebView->show();



            if (!tempFile.remove()) writeStatus("Unable to delete temp file. Check file permisons", WARNING);
            else writeStatus("Service Map loaded succesfuly. Congrats!", NO_ERROR);

        }

        //calculate statistics
        showStatistics(serviceFileName);



    }
}

//View the content of the  tx selected file in a new window
void MainWindow::on_txRawDataFileViewpushButton_clicked()
{
    if (!ui->txRawDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToWatchName=currentDir+ "/"+ RAW_DIR+"/"+txRawDataFilesModel->data(ui->txRawDataFileslistView->currentIndex()).toString();

        //try to open file
        QFile fileToWatch(fileToWatchName);
        if (!fileToWatch.open(QIODevice::ReadOnly)) {
            writeStatus("Unable to open file "+fileToWatchName, ERROR);
        } else
        {

            const QString fileText(QString::fromUtf8(fileToWatch.readAll()));
            fileToWatch.close();

            DialogViewfile viewFileDialog(fileText);

            viewFileDialog.exec();
        }



    }
}

//View the content of the rx selected file in a new window
void MainWindow::on_txImportedFilesViewpushButton_clicked()
{
    if (!ui->txImportedDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToWatchName=currentDir+ "/"+ TX_IMPORTED_DIR+"/"+txImportedFilesModel->data(ui->txImportedDataFileslistView->currentIndex()).toString();

        //try to open file
        QFile fileToWatch(fileToWatchName);
        if (!fileToWatch.open(QIODevice::ReadOnly)) {
            writeStatus("Unable to open file "+fileToWatchName, ERROR);
        } else
        {
            const QString fileText(QString::fromUtf8(fileToWatch.readAll()));
            fileToWatch.close();

            DialogViewfile viewFileDialog(fileText);

            viewFileDialog.exec();
        }

    }

}

//View the content of the rx raw selected file in a new window
void MainWindow::on_rxRawDataFileViewpushButton_clicked()
{
    if (!ui->rxRawDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToWatchName=currentDir+ "/"+ RAW_DIR+"/"+rxRawDataFilesModel->data(ui->rxRawDataFileslistView->currentIndex()).toString();

        //try to open file
        QFile fileToWatch(fileToWatchName);
        if (!fileToWatch.open(QIODevice::ReadOnly)) {
            writeStatus("Unable to open file "+fileToWatchName, ERROR);
        } else
        {

            const QString fileText(QString::fromUtf8(fileToWatch.readAll()));
            fileToWatch.close();

            DialogViewfile viewFileDialog(fileText);

            viewFileDialog.exec();
        }


    }

}

//View the content of the rx selected file in a new window
void MainWindow::on_rxImportedFilesViewpushButton_clicked()
{
    if (!ui->rxImportedDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToWatchName=currentDir+ "/"+ RX_IMPORTED_DIR+"/"+rxImportedFilesModel->data(ui->rxImportedDataFileslistView->currentIndex()).toString();

        //try to open file
        QFile fileToWatch(fileToWatchName);
        if (!fileToWatch.open(QIODevice::ReadOnly)) {
            writeStatus("Unable to open file "+fileToWatchName, ERROR);
        } else
        {
            const QString fileText(QString::fromUtf8(fileToWatch.readAll()));
            fileToWatch.close();

            DialogViewfile viewFileDialog(fileText);

            viewFileDialog.exec();
        }

    }

}

//View the content of the processed selected file in a new window
void MainWindow::on_ProcessViewpushButton_clicked()
{
    if (!ui->ProcessOutlistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToWatchName=currentDir+ "/"+ OUT_DIR+"/"+processProcessedDataFilesModel->data(ui->ProcessOutlistView->currentIndex()).toString();

        //try to open file
        QFile fileToWatch(fileToWatchName);
        if (!fileToWatch.open(QIODevice::ReadOnly)) {
            writeStatus("Unable to open file "+fileToWatchName, ERROR);
        } else
        {

            const QString fileText(QString::fromUtf8(fileToWatch.readAll()));
            fileToWatch.close();

            DialogViewfile viewFileDialog(fileText);

            viewFileDialog.exec();
        }


    }

}

//Delete selected file
void MainWindow::on_txRawDataFilesDeletepushButton_clicked()
{
    QMessageBox msgBox(this);
    if (!ui->txRawDataFileslistView->currentIndex().isValid()){

        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file to be deleted");
        msgBox.exec();
    } else
    {
        QString fileToRemove=txRawDataFilesModel->data(ui->txRawDataFileslistView->currentIndex()).toString();
        msgBox.setText("You are about to DELETE a file");
        msgBox.setInformativeText("Do you really want to permanently remove this file from the system?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret=msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            if (!txRawDataFilesModel->remove(ui->txRawDataFileslistView->currentIndex())) writeStatus("Unable to remove file "+fileToRemove,ERROR);
            else writeStatus("File "+fileToRemove+" removed",NO_ERROR);
            break;
        default:
            break;


        }

    }
}

//Delete selected file
void MainWindow::on_tximportedFilesDeletepushButton_clicked()
{
    QMessageBox msgBox(this);
    if (!ui->txImportedDataFileslistView->currentIndex().isValid()){

        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file to be deleted");
        msgBox.exec();
    } else
    {
        QString fileToRemove=txImportedFilesModel->data(ui->txImportedDataFileslistView->currentIndex()).toString();
        msgBox.setText("You are about to DELETE a file");
        msgBox.setInformativeText("Do you really want to permanently remove this file from the system?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret=msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            if (!txImportedFilesModel->remove(ui->txImportedDataFileslistView->currentIndex())) writeStatus("Unable to remove file "+fileToRemove,ERROR);
            else writeStatus("File "+fileToRemove+" removed",NO_ERROR);
            break;
        default:
            break;


        }

    }

}

//Delete selected file
void MainWindow::on_rxRawDataFilesDeletepushButton_clicked()
{
    QMessageBox msgBox(this);
    if (!ui->rxRawDataFileslistView->currentIndex().isValid()){

        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file to be deleted");
        msgBox.exec();
    } else
    {
        QString fileToRemove=rxRawDataFilesModel->data(ui->rxRawDataFileslistView->currentIndex()).toString();
        msgBox.setText("You are about to DELETE a file");
        msgBox.setInformativeText("Do you really want to permanently remove this file from the system?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret=msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            if (!rxRawDataFilesModel->remove(ui->rxRawDataFileslistView->currentIndex())) writeStatus("Unable to remove file "+fileToRemove,ERROR);
            else writeStatus("File "+fileToRemove+" removed",NO_ERROR);
            break;
        default:
            break;


        }

    }

}

//Delete selected file
void MainWindow::on_importedFilesDeletepushButton_clicked()
{
    QMessageBox msgBox(this);
    if (!ui->rxImportedDataFileslistView->currentIndex().isValid()){

        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file to be deleted");
        msgBox.exec();
    } else
    {
        QString fileToRemove=rxImportedFilesModel->data(ui->rxImportedDataFileslistView->currentIndex()).toString();
        msgBox.setText("You are about to DELETE a file");
        msgBox.setInformativeText("Do you really want to permanently remove this file from the system?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret=msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            if (!rxImportedFilesModel->remove(ui->rxImportedDataFileslistView->currentIndex())) writeStatus("Unable to remove file "+fileToRemove,ERROR);
            else writeStatus("File "+fileToRemove+" removed",NO_ERROR);
            break;
        default:
            break;


        }

    }
}

//Delete selected file
void MainWindow::on_ProcessDeletepushButton_clicked()
{
    QMessageBox msgBox(this);
    if (!ui->ProcessOutlistView->currentIndex().isValid()){

        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file to be deleted");
        msgBox.exec();
    } else
    {
        QString fileToRemove=processProcessedDataFilesModel->data(ui->ProcessOutlistView->currentIndex()).toString();
        msgBox.setText("You are about to DELETE a file");
        msgBox.setInformativeText("Do you really want to permanently remove this file from the system?");
        msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        int ret=msgBox.exec();
        switch (ret)
        {
        case QMessageBox::Ok:
            if (!processProcessedDataFilesModel->remove(ui->ProcessOutlistView->currentIndex())) writeStatus("Unable to remove file "+fileToRemove,ERROR);
            else writeStatus("File "+fileToRemove+" removed",NO_ERROR);
            break;
        default:
            break;


        }

    }
}

//Rename selected file
void MainWindow::on_txRawDataFileRenamepushButton_clicked()
{

    QString configFileName=currentDir+"/config.ini";

    QSettings settings(configFileName,QSettings::IniFormat);

    if (!ui->txRawDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToRenameName=txRawDataFilesModel->data(ui->txRawDataFileslistView->currentIndex()).toString();
        QFileInfo fileToRenameInfo(currentDir+ "/"+ RAW_DIR+"/"+fileToRenameName);
        //only pass the name, not the extension
        DialogRenameFile renameFileDialog(fileToRenameInfo.completeBaseName());

        if(renameFileDialog.exec())
        {
           if (!renameFileDialog.getNewFilename().isEmpty())
           {
               QString extension=settings.value("Extensions/txRawData").toString();
               QFile fileToRename(currentDir+ "/"+ RAW_DIR+"/"+fileToRenameName);
               QFile outputFile(currentDir+"/"+RAW_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);
               QFileInfo outputFileInfo(currentDir+"/"+RAW_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);

                if (!fileToRename.rename(outputFile.fileName())) writeStatus("Unable to rename file "+outputFile.fileName(),ERROR);
                else writeStatus("File "+fileToRenameInfo.completeBaseName()+" renamed to "+outputFileInfo.completeBaseName(),NO_ERROR);
            } else writeStatus("Invalid Name. File not renamed", ERROR);
        }


    }

}

//Rename selected file
void MainWindow::on_txImportedFileRenamepushButton_clicked()
{
    QString configFileName=currentDir+"/config.ini";

    QSettings settings(configFileName,QSettings::IniFormat);

    if (!ui->txImportedDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToRenameName=txImportedFilesModel->data(ui->txImportedDataFileslistView->currentIndex()).toString();
        QFileInfo fileToRenameInfo(currentDir+ "/"+ TX_IMPORTED_DIR+"/"+fileToRenameName);
        //only pass the name, not the extension
        DialogRenameFile renameFileDialog(fileToRenameInfo.completeBaseName());

        if(renameFileDialog.exec())
        {
           if (!renameFileDialog.getNewFilename().isEmpty())
           {
               QString extension=settings.value("Extensions/txImportedData").toString();
               QFile fileToRename(currentDir+ "/"+ TX_IMPORTED_DIR+"/"+fileToRenameName);
               QFile outputFile(currentDir+"/"+TX_IMPORTED_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);
               QFileInfo outputFileInfo(currentDir+"/"+TX_IMPORTED_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);

                if (!fileToRename.rename(outputFile.fileName())) writeStatus("Unable to rename file "+outputFile.fileName(),ERROR);
                else writeStatus("File "+fileToRenameInfo.completeBaseName()+" renamed to "+outputFileInfo.completeBaseName(),NO_ERROR);
            } else writeStatus("Invalid Name. File not renamed", ERROR);
        }


    }
}

//Rename selected file
void MainWindow::on_rxRawDataFileRenamepushButton_clicked()
{
    QString configFileName=currentDir+"/config.ini";

    QSettings settings(configFileName,QSettings::IniFormat);

    if (!ui->rxRawDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToRenameName=rxRawDataFilesModel->data(ui->rxRawDataFileslistView->currentIndex()).toString();
        QFileInfo fileToRenameInfo(currentDir+ "/"+ RAW_DIR+"/"+fileToRenameName);
        //only pass the name, not the extension
        DialogRenameFile renameFileDialog(fileToRenameInfo.completeBaseName());

        if(renameFileDialog.exec())
        {
           if (!renameFileDialog.getNewFilename().isEmpty())
           {
               QString extension=settings.value("Extensions/rxRawData").toString();
               QFile fileToRename(currentDir+ "/"+ RAW_DIR+"/"+fileToRenameName);
               QFile outputFile(currentDir+"/"+RAW_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);
               QFileInfo outputFileInfo(currentDir+"/"+RAW_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);

                if (!fileToRename.rename(outputFile.fileName())) writeStatus("Unable to rename file "+outputFile.fileName(),ERROR);
                else writeStatus("File "+fileToRenameInfo.completeBaseName()+" renamed to "+outputFileInfo.completeBaseName(),NO_ERROR);
            } else writeStatus("Invalid Name. File not renamed", ERROR);
        }


    }

}

//Rename selected file
void MainWindow::on_rxImportedFileRenamepushButton_clicked()
{
    QString configFileName=currentDir+"/config.ini";

    QSettings settings(configFileName,QSettings::IniFormat);

    if (!ui->rxImportedDataFileslistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToRenameName=rxImportedFilesModel->data(ui->rxImportedDataFileslistView->currentIndex()).toString();
        QFileInfo fileToRenameInfo(currentDir+ "/"+ RX_IMPORTED_DIR+"/"+fileToRenameName);
        //only pass the name, not the extension
        DialogRenameFile renameFileDialog(fileToRenameInfo.completeBaseName());

        if(renameFileDialog.exec())
        {
           if (!renameFileDialog.getNewFilename().isEmpty())
           {
               QString extension=settings.value("Extensions/rxImportedData").toString();
               QFile fileToRename(currentDir+ "/"+ RX_IMPORTED_DIR+"/"+fileToRenameName);
               QFile outputFile(currentDir+"/"+RX_IMPORTED_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);
               QFileInfo outputFileInfo(currentDir+"/"+RX_IMPORTED_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);

                if (!fileToRename.rename(outputFile.fileName())) writeStatus("Unable to rename file "+outputFile.fileName(),ERROR);
                else writeStatus("File "+fileToRenameInfo.completeBaseName()+" renamed to "+outputFileInfo.completeBaseName(),NO_ERROR);
            } else writeStatus("Invalid Name. File not renamed", ERROR);
        }


    }

}

//Rename selected file
void MainWindow::on_processRenamepushButton_clicked()
{
    QString configFileName=currentDir+"/config.ini";

    QSettings settings(configFileName,QSettings::IniFormat);

    if (!ui->ProcessOutlistView->currentIndex().isValid()){
        QMessageBox msgBox(this);
        msgBox.setIcon(QMessageBox::Information);
        msgBox.setText("Please select a file");
        msgBox.exec();
    } else
    {
        QString fileToRenameName=processProcessedDataFilesModel->data(ui->ProcessOutlistView->currentIndex()).toString();
        QFileInfo fileToRenameInfo(currentDir+ "/"+ OUT_DIR+"/"+fileToRenameName);
        //only pass the name, not the extension
        DialogRenameFile renameFileDialog(fileToRenameInfo.completeBaseName());

        if(renameFileDialog.exec())
        {
           if (!renameFileDialog.getNewFilename().isEmpty())
           {
               QString extension=settings.value("Extensions/processedData").toString();
               QFile fileToRename(currentDir+ "/"+ OUT_DIR+"/"+fileToRenameName);
               QFile outputFile(currentDir+"/"+OUT_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);
               QFileInfo outputFileInfo(currentDir+"/"+OUT_DIR+"/"+renameFileDialog.getNewFilename()+"."+extension);

                if (!fileToRename.rename(outputFile.fileName())) writeStatus("Unable to rename file "+outputFile.fileName(),ERROR);
                else writeStatus("File "+fileToRenameInfo.completeBaseName()+" renamed to "+outputFileInfo.completeBaseName(),NO_ERROR);
            } else writeStatus("Invalid Name. File not renamed", ERROR);
        }


    }

}

//Download file from SCAN-server
void MainWindow::on_getFromServerpushButton_clicked()
{
    DialogDownloadFromServer downloadFromServerDialog;

    downloadFromServerDialog.exec();
}

//Show message about the progress of loading a map
void MainWindow::updateLoadProgress(int progress)
{
    QString progressS=QString::number(progress);
    writeStatus("Loading Map: "+progressS+" %",NO_ERROR);
}

//Show message about the final status of loading a map
void MainWindow::updateLoadFinished(bool ok)
{
    if (!ok){
        writeStatus("Error loading Map. Check network connection", ERROR);
    } else
    {
        writeStatus("Map loaded", NO_ERROR);
    }
}

//Show statistis from geojson file
void MainWindow::showStatistics(QString serviceFileName)
{

    QJsonDocument inputRxJsonDocument;
    QJsonArray featuresJsonArray;
    QJsonObject featureJsonObject, inputJsonObject;
    QFile rxFile(serviceFileName);

    double rssimax=-200;
    double rssimin=200;
    double snrmax=-200;
    double snrmin=200;
    double tempDouble;
    double station0=0;
    double station1=0;
    double station2=0;
    double station3=0;
    double totalTx=0;
    double totalRx=0;




    //Try to open input file
    if (!rxFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        //fill statistic with error
        printErrorOnStatistics();

    } else
    {
        //load data (jsonObject) from file
        QString jsonData;
        jsonData=rxFile.readAll();
        rxFile.close();

        //Check if it contains data and is an object
        inputRxJsonDocument=QJsonDocument::fromJson(jsonData.toUtf8());
        if (inputRxJsonDocument.isNull() || !inputRxJsonDocument.isObject() )
        {
            printErrorOnStatistics();
        } else
        {
            //load object from json document
            inputJsonObject=inputRxJsonDocument.object();
            //load features array
            featuresJsonArray=inputJsonObject.value("features").toArray();
            for(int i=0;i<featuresJsonArray.size();i++)
            {
                totalTx++;
                featureJsonObject=featuresJsonArray.at(i).toObject().value("properties").toObject();
                tempDouble=featureJsonObject.value("stations").toDouble();
                if (tempDouble==0) station0++;
                else if (tempDouble==1)
                {
                    station1++;
                    totalRx++;
                }
                else if (tempDouble==2)
                {
                    station2++;
                    totalRx++;
                }
                else
                {
                    station3++;
                    totalRx++;
                }

                if (tempDouble > 0)
                {

                    //get this values only if the message has been received
                    tempDouble=featureJsonObject.value("rssiMax").toDouble();
                    if (tempDouble > rssimax) rssimax=tempDouble;
                    tempDouble=featureJsonObject.value("rssiMin").toDouble();
                    if (tempDouble < rssimin) rssimin=tempDouble;
                    tempDouble=featureJsonObject.value("snrMax").toDouble();
                    if (tempDouble > snrmax) snrmax=tempDouble;
                    tempDouble=featureJsonObject.value("snrMin").toDouble();
                    if (tempDouble < snrmin) snrmin=tempDouble;
                }

            }

            QString tempString;

            //print statistics on screen
            ui->totalTxlabel->setText(QString::number(totalTx));
            ui->totalRxlabel->setText(QString::number(totalRx));


            tempString.setNum(totalRx*100/totalTx,'f',1);
            ui->serviceOklabel->setText(tempString+" %");
            tempString.setNum(100-totalRx*100/totalTx,'f',1);
            ui->serviceNOKlabel->setText(tempString+" %");

            tempString.setNum(rssimax,'f',1);
            ui->rssiMaxlabel->setText(tempString);
            tempString.setNum(rssimin,'f',1);
            ui->rssiMinlabel->setText(tempString);
            tempString.setNum(snrmax,'f',1);
            ui->snrMaxlabel->setText(tempString);
            tempString.setNum(snrmin,'f',1);
            ui->snrMinlabel->setText(tempString);

            ui->rx0stationlabel->setText(QString::number(station0));
            ui->rx1stationlabel->setText(QString::number(station1));
            ui->rx2stationlabel->setText(QString::number(station2));
            ui->rx3stationlabel->setText(QString::number(station3));
        }

    }


}

//Show error on statistic section
void MainWindow::printErrorOnStatistics()
{
    ui->totalTxlabel->setText("Err");
    ui->totalRxlabel->setText("Err");
    ui->serviceOklabel->setText("Err");
    ui->serviceNOKlabel->setText("Err");
    ui->rssiMaxlabel->setText("Err");
    ui->rssiMinlabel->setText("Err");
    ui->snrMaxlabel->setText("Err");
    ui->snrMinlabel->setText("Err");
    ui->rx0stationlabel->setText("Err");
    ui->rx1stationlabel->setText("Err");
    ui->rx2stationlabel->setText("Err");
    ui->rx3stationlabel->setText("Err");


}

//Clear statistics section
void MainWindow::clearStatistics()
{
    //clear statistics from screen
    ui->totalTxlabel->setText("");
    ui->totalRxlabel->setText("");

    ui->serviceOklabel->setText("");
    ui->serviceNOKlabel->setText("");

    ui->rssiMaxlabel->setText("");
    ui->rssiMinlabel->setText("");
    ui->snrMaxlabel->setText("");
    ui->snrMinlabel->setText("");

    ui->rx0stationlabel->setText("");
    ui->rx1stationlabel->setText("");
    ui->rx2stationlabel->setText("");
    ui->rx3stationlabel->setText("");

}

//Download file from SCAN-device
void MainWindow::on_getFromDevicepushButton_clicked()
{
    DialogDownloadFromDevice downloadFromDeviceDialog;

    downloadFromDeviceDialog.exec();

}
