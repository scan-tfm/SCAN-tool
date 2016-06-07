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

#include "payloadmanager.h"


PayloadManager::PayloadManager(QObject *parent) : QObject(parent)
{

}

//Import raw data file form raw directory to rx directory.
//Returns error code.
int PayloadManager::importRxRawDataFile(QString fileToImport, QString destinationDir)
{
    QFile file(fileToImport);

    QJsonDocument myJsonDocument, outputJsonDocument;
    QJsonArray myJsonArray, outputJsonArray;
    QJsonObject myJsonObject, outputJsonObject;

    //Try opening input file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return UNABLE_OPEN_IN_FILE;
    //To store rawData from input file
    QString rawData, outputJsonData;
    rawData=file.readAll();
    file.close();

    //convert rawData to a properly formed json stream
    if (!rawData.isEmpty())
    {
        if (rawData.endsWith('\n'))
        {
            //remove last \n
            rawData.remove(rawData.lastIndexOf('\n'),1);
            //replace every return by commas
            rawData.replace('\n',',');
            //insert trailer and ending [] to construct a json array
            rawData.prepend('[');
            rawData.append(']');
            myJsonDocument=QJsonDocument::fromJson(rawData.toUtf8());
            if (!myJsonDocument.isNull())
            {
                myJsonArray=myJsonDocument.array();



                for(int i=0;i<myJsonArray.size();i++)
                {
                    myJsonObject=myJsonArray.at(i).toObject();
                    if (decodeRawJsonData(myJsonObject,&outputJsonObject))
                    {

                        //save jsonobject to output json objet
                        outputJsonArray.insert(i,outputJsonObject);


                    } else return JSON_OBJECT_ERROR;

                }

                outputJsonDocument.setArray(outputJsonArray);
                //save outputFile
                QString timeStamp=QDateTime::currentDateTime().toString("dd-MM-yyyy-hhmmss-");
                QString fileName(QApplication::applicationDirPath()+"/"+destinationDir+"/"+timeStamp+"SCAN-server.json");
                QFile outputFile(fileName);
                if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) return UNABLE_OPEN_OUT_FILE;
                outputFile.write(outputJsonDocument.toJson());
                outputFile.close();


            } else return JSON_PARSE_ERROR;



        } else return FORMAT_ERROR;

    } else
    {
        return EMPTY_FILE;
    }


    return NO_ERROR;
}

//Import raw data file form raw directory to tx directory.
//Returns error code.
int PayloadManager::importTxRawDataFile(QString fileToImport, QString destinationDir)
{
    QFile file(fileToImport);

    QJsonDocument outputJsonDocument;
    QJsonArray outputJsonArray;
    QJsonObject outputJsonObject;

    //Try opening input file
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return UNABLE_OPEN_IN_FILE;

    QTextStream inputStream(&file);
    QString line="";
    int i;
    for (i=0;!inputStream.atEnd();i++){
        line=inputStream.readLine();
        if (line!="EOF")
        {

            //Fill Json object
            outputJsonObject.insert("txID",line.section(',',0,0));
            //get power from second field: first 4 bits
            QString field=line.section(',',1,1);
            int temp=field.toInt() & 0x0F;
            QString tempS=QString::number(temp);
            outputJsonObject.insert("power",tempS);

            //get txACK from second field: bit 5
            if (field.toInt() & 0x10)
            {
                outputJsonObject.insert("txACK",true);
            } else
            {
                outputJsonObject.insert("txACK",false);
            }

            //get rxACK from second field: bit 6
            if (field.toInt() & 0x20)
            {
                outputJsonObject.insert("rxACK",true);
            } else
            {
                outputJsonObject.insert("rxACK",false);
            }


            outputJsonObject.insert("latitude",fromRawGeopositionToDegrees(line.section(',',2,2)));
            outputJsonObject.insert("longitude",fromRawGeopositionToDegrees(line.section(',',3,3)));

            //Fill Json array
            outputJsonArray.insert(i,outputJsonObject);

        }
    }

    if (i!=0)
    {
        //Fill Json document and save file
        outputJsonDocument.setArray(outputJsonArray);
        //save outputFile
        QString timeStamp=QDateTime::currentDateTime().toString("dd-MM-yyyy-hhmmss-");
        QString fileName(QApplication::applicationDirPath()+"/"+destinationDir+"/"+timeStamp+"SCAN-device.json");
        QFile outputFile(fileName);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) return UNABLE_OPEN_OUT_FILE;
        outputFile.write(outputJsonDocument.toJson());
        outputFile.close();
    } else
    {
        file.close();
        return EMPTY_FILE;
    }


    file.close();
    return NO_ERROR;
}

//Decodes an inputObject of type Json to a decoded outputObject of type Json
bool PayloadManager::decodeRawJsonData(QJsonObject inputObject, QJsonObject *outputObject)
{
    decodedPayload decodedPayloadResult;
    //check integrity of data
    if (inputObject.contains("time")&&inputObject.contains("device")&&inputObject.contains("duplicate")&&inputObject.contains("snr")
            &&inputObject.contains("rssi")&&inputObject.contains("data")&&inputObject.contains("station"))
    {
        ////// time: convert time from unix time to utc
        QDateTime time;
        uint unixtime=inputObject.value("time").toString().toInt();
        time.setTime_t(unixtime);
        //Store formated time on output object
        outputObject->insert("time",time.toString("dd-MM-yyyy hh:mm:ss"));

        ///// device
        outputObject->insert("device",inputObject.value("device").toString());

        ///// duplicate
        outputObject->insert("duplicate",inputObject.value("duplicate").toString());

        ///// station
        outputObject->insert("station",inputObject.value("station").toString());

        ///// snr
        outputObject->insert("snr",inputObject.value("snr").toString());

        ///// rssi
        outputObject->insert("rssi",inputObject.value("rssi").toString());

        ///// data
        //outputObject->insert("data",inputObject.value("data").toString());
        decodedPayloadResult=getDecodedPayload(inputObject.value("data").toString());
        outputObject->insert("txID",decodedPayloadResult.txID);
        outputObject->insert("power",decodedPayloadResult.power);
        outputObject->insert("latitude",decodedPayloadResult.latitude);
        outputObject->insert("longitude",decodedPayloadResult.longitude);

        /*
        outputObject->insert("txACK",decodedPayloadResult.txACK);
        outputObject->insert("rxACK",decodedPayloadResult.rxACK);
        */



        return true;
    } else return false;

}

//Translates from raw data position, to position in degrees
QString PayloadManager::fromRawGeopositionToDegrees(QString rawGeo)
{
    float degrees,minutes;
    QString result;
    QString tempString=rawGeo;
    minutes=tempString.right(7).toFloat()/100000;
    degrees=tempString.remove(tempString.size()-7,7).toFloat();
    if (degrees >=0) degrees=degrees+minutes/60;
    else degrees=degrees-minutes/60;
    result.setNum(degrees,'f', 12);
    return result;

}

//Process input json files and get output processed geojson file
int PayloadManager::processData(QString rxFileName, QString rxDir, QModelIndexList txFileNameList, QFileSystemModel *processTxDataFilesModel, QString txDir, QString outDir)
{
    QString currentDir=QApplication::applicationDirPath();
    QJsonDocument outputJsonDocument, inputRxJsonDocument, inputTxJsonDocument;
    QJsonArray tempJsonArray, featuresJsonArray, coordinatesJsonArray;
    QJsonObject tempJsonObject, mainJsonObject, featureJsonObject;
    QFile rxFile(rxDir+"/"+rxFileName);
    QFile txFile;

    if (!rxFile.open(QIODevice::ReadOnly | QIODevice::Text)) return UNABLE_OPEN_IN_FILE;
    QString jsonData;
    jsonData=rxFile.readAll();
    rxFile.close();

    //load data (array) to jsondocument
    inputRxJsonDocument=QJsonDocument::fromJson(jsonData.toUtf8());
    if (inputRxJsonDocument.isNull() || !inputRxJsonDocument.isArray())
    {
        return JSON_PARSE_ERROR;
    } else
    {

        foreach (QModelIndex i, txFileNameList)
        {
            //qDebug()<<processTxDataFilesModel->fileName(i);
            //read jsonData fromFile
            int indexArray=0;
            txFile.setFileName(currentDir+"/"+txDir+"/"+processTxDataFilesModel->fileName(i));
            if (!txFile.open(QIODevice::ReadOnly | QIODevice::Text)) return UNABLE_OPEN_IN_FILE;
            jsonData=txFile.readAll();
            txFile.close();
            //load data (array) to jsondocument
            inputTxJsonDocument=QJsonDocument::fromJson(jsonData.toUtf8());

            if (!inputTxJsonDocument.isArray()) return JSON_PARSE_ERROR;
            //read array
            tempJsonArray=inputTxJsonDocument.array();
            for(int i=0;i<tempJsonArray.size();i++)
            {
                tempJsonObject=tempJsonArray.at(i).toObject();
                featureJsonObject=getFeature(tempJsonObject,inputRxJsonDocument.array());


                //store the feature inside array
                featuresJsonArray.insert(indexArray,featureJsonObject);
                indexArray++;



            }




        }

        //Create Final Json object output
        mainJsonObject.insert("type","FeatureCollection");
        mainJsonObject.insert("features",featuresJsonArray);

        //Fill Json document and save file
        outputJsonDocument.setObject(mainJsonObject);
        //save outputFile
        QString timeStamp=QDateTime::currentDateTime().toString("dd-MM-yyyy-hhmmss-");
        QString fileName(QApplication::applicationDirPath()+"/"+outDir+"/"+timeStamp+"SCAN-processed.geojson");
        QFile outputFile(fileName);
        if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) return UNABLE_OPEN_OUT_FILE;
        outputFile.write(outputJsonDocument.toJson());
        outputFile.close();

    }

    return NO_ERROR;

}

//Get build a geojson feature from imput data
QJsonObject PayloadManager::getFeature(QJsonObject txJsonObject, QJsonArray rxJsonArray)
{
    QJsonObject FeatureToReturn, tempJsonObject;

    int numberOfTimesReceived=0;
    //QList<QString> stations, rssi, snr;
    double rssimax=-200;
    double rssimin=200;
    double snrmax=-200;
    double snrmin=200;
    double tempDouble;
    QString txID, time;
    //this is the string to be found in order to check if there has been reception
    txID=txJsonObject.value("txID").toString();
    for (int i=0;i<rxJsonArray.size();i++)
    {
        tempJsonObject=rxJsonArray.at(i).toObject();

        if (txID==tempJsonObject.value("txID").toString()){
            //GOOD reception
            numberOfTimesReceived++;
            time=tempJsonObject.value("time").toString();


            //Update statistics
            tempDouble=tempJsonObject.value("rssi").toString().toDouble();
            if (tempDouble > rssimax) rssimax=tempDouble;
            if (tempDouble < rssimin) rssimin=tempDouble;
            tempDouble=tempJsonObject.value("snr").toString().toDouble();
            if (tempDouble > snrmax) snrmax=tempDouble;
            if (tempDouble < snrmin) snrmin=tempDouble;




        }

    }
    //fill the returned feature

    QString tempStation, tempLatitude, tempLongitude;

    QJsonObject propertiesJsonObject,geometryJsonObject;
    QJsonArray coordinatesJsonArray;
    propertiesJsonObject.insert("txID",txID);
    propertiesJsonObject.insert("power",txJsonObject.value("power").toString());
    propertiesJsonObject.insert("stations",numberOfTimesReceived);


    QJsonValue longitudeValue(txJsonObject.value("longitude").toString().toDouble());
    QJsonValue latitudeValue(txJsonObject.value("latitude").toString().toDouble());



    coordinatesJsonArray.insert(0,longitudeValue);
    coordinatesJsonArray.insert(1,latitudeValue);

    geometryJsonObject.insert("type","Point");
    geometryJsonObject.insert("coordinates",coordinatesJsonArray);


    if(numberOfTimesReceived==0)
    {
        propertiesJsonObject.insert("service","NOK");
        tempStation=QString::number(propertiesJsonObject.value("stations").toInt());
        propertiesJsonObject.insert("popupContent","TXID: "+ propertiesJsonObject.value("txID").toString() + "<br>"+
                                    "Service: NOK" + "<br>" +
                                    "Transmitted Power: " + propertiesJsonObject.value("power").toString()+" dBm"+ "<br>" +
                                    "Stations: " + tempStation + "<br>" +
                                    "Latitude: "+ tempLatitude.setNum(latitudeValue.toDouble(),'f',12) + "<br>" +
                                    "Longitude: " + tempLongitude.setNum(longitudeValue.toDouble(),'f',12));
    } else
    {
        QString txACKValue, rxACKValue;
        //prepare the ACK string to be included in popup content
        if (txJsonObject.value("txACK").toBool()) txACKValue="Yes";
        else txACKValue="No";

        if (txJsonObject.value("rxACK").toBool()) rxACKValue="Yes";
        else rxACKValue="No";


        propertiesJsonObject.insert("service","OK");
        propertiesJsonObject.insert("rssiMax",rssimax);
        propertiesJsonObject.insert("rssiMin",rssimin);
        propertiesJsonObject.insert("snrMax",snrmax);
        propertiesJsonObject.insert("snrMin",snrmin);
        tempStation=QString::number(propertiesJsonObject.value("stations").toInt());
        propertiesJsonObject.insert("popupContent","TXID: "+ propertiesJsonObject.value("txID").toString() + "<br>"+
                                    "Service: OK" + "<br>" +
                                    "Transmitted Power: " + propertiesJsonObject.value("power").toString()+" dBm"+ "<br>" +
                                    "Transmission with ACK: " + txACKValue + "<br>" +
                                    "ACK received: " + rxACKValue + "<br>" +
                                    "Stations: " + tempStation + "<br>" +
                                    "Latitude: "+ tempLatitude.setNum(latitudeValue.toDouble(),'f',12) + "<br>" +
                                    "Longitude: " + tempLongitude.setNum(longitudeValue.toDouble(),'f',12) +"<br>" +
                                    "RSSI max: " + QString::number(rssimax)+" dBm"+ "<br>"+
                                    "RSSI min: "+ QString::number(rssimin)+" dBm"+ "<br>"+
                                    "SNR max: "+ QString::number(snrmax)+" dB"+ "<br>"+
                                    "SNR min: "+QString::number(snrmin)+" dB");




    }

    FeatureToReturn.insert("type","Feature");
    FeatureToReturn.insert("properties",propertiesJsonObject);
    FeatureToReturn.insert("geometry",geometryJsonObject);


    return FeatureToReturn;

}

//Decode payload from received message
PayloadManager::decodedPayload PayloadManager::getDecodedPayload(QString codedPayload)
{
    decodedPayload decodedStructToReturn;
    //Union to manage txId byte order (little vs big endian)
    union {
        quint8 bytePosition[2];
        quint16 value;

    } unionTxID;

    //Union to manage Power value. In this case, only one byte so no little/big endian matter
    union {
        quint8 bytePosition;
    } unionPower;

    //Union to manage Latitude byte order (little vs big endian). Note that it is signed int
    union {
        quint8 bytePosition[4];
        qint32 value;
    } unionLatitude;

    //Union to manage Longitude byte order (little vs big endian). Note that it is signed int
    union {
        quint8 bytePosition[4];
        qint32 value;
    } unionLongitude;

    QByteArray payloadHex;
    payloadHex=QByteArray::fromHex(codedPayload.toLatin1());

    //Check endianess before fill struct
    if (QSysInfo::Endian()==0)
    {
        //Arquitecture is big endian (i.e. Linux)
        unionTxID.bytePosition[0]=payloadHex[0];
        unionTxID.bytePosition[1]=payloadHex[1];
        decodedStructToReturn.txID=QString::number(unionTxID.value);
        //qDebug()<< decodedStructToReturn.txID;

        //Get power from first 4 bits. Rest of bits for future uses
        unionPower.bytePosition=payloadHex[2] & 0x0F;

        decodedStructToReturn.power=QString::number(unionPower.bytePosition);
        //qDebug()<<decodedStructToReturn.power;

        unionLatitude.bytePosition[0]=payloadHex[3];
        unionLatitude.bytePosition[1]=payloadHex[4];
        unionLatitude.bytePosition[2]=payloadHex[5];
        unionLatitude.bytePosition[3]=payloadHex[6];
        decodedStructToReturn.latitude=QString::number(unionLatitude.value);
        //qDebug()<<decodedStructToReturn.latitude;

        unionLongitude.bytePosition[0]=payloadHex[7];
        unionLongitude.bytePosition[1]=payloadHex[8];
        unionLongitude.bytePosition[2]=payloadHex[9];
        unionLongitude.bytePosition[3]=payloadHex[10];
        decodedStructToReturn.longitude=QString::number(unionLongitude.value);
        //qDebug()<<decodedStructToReturn.longitude;
    } else
    {
        //arquitecture is little endian (i.e. Windows)
        unionTxID.bytePosition[0]=payloadHex[1];
        unionTxID.bytePosition[1]=payloadHex[0];
        decodedStructToReturn.txID=QString::number(unionTxID.value);


        //Get power from first 4 bits. Rest of bits for future uses
        unionPower.bytePosition=payloadHex[2] & 0x0F;

        decodedStructToReturn.power=QString::number(unionPower.bytePosition);


        unionLatitude.bytePosition[0]=payloadHex[6];
        unionLatitude.bytePosition[1]=payloadHex[5];
        unionLatitude.bytePosition[2]=payloadHex[4];
        unionLatitude.bytePosition[3]=payloadHex[3];
        decodedStructToReturn.latitude=QString::number(unionLatitude.value);


        unionLongitude.bytePosition[0]=payloadHex[10];
        unionLongitude.bytePosition[1]=payloadHex[9];
        unionLongitude.bytePosition[2]=payloadHex[8];
        unionLongitude.bytePosition[3]=payloadHex[7];
        decodedStructToReturn.longitude=QString::number(unionLongitude.value);


    }

    //format Latitude degrees, minutes
    float degrees,minutes;
    QString tempString=decodedStructToReturn.latitude;
    minutes=tempString.right(7).toFloat()/100000;
    degrees=tempString.remove(tempString.size()-7,7).toFloat();
    if (degrees >=0) degrees=degrees+minutes/60;
    else degrees=degrees-minutes/60;
    decodedStructToReturn.latitude.setNum(degrees,'f', 12);


    //format Longitude degrees, minutes

    tempString=decodedStructToReturn.longitude;
    minutes=tempString.right(7).toFloat()/100000;
    degrees=tempString.remove(tempString.size()-7,7).toFloat();
    if (degrees >=0) degrees=degrees+minutes/60;
    else degrees=degrees-minutes/60;
    decodedStructToReturn.longitude.setNum(degrees,'f', 12);

    return decodedStructToReturn;

}

