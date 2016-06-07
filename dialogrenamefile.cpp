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

#include "dialogrenamefile.h"
#include "ui_dialogrenamefile.h"

DialogRenameFile::DialogRenameFile(QString inputFileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRenameFile)
{
    ui->setupUi(this);

    //File text field with file name
    this->ui->lineEdit->setText(inputFileName);
}

QString DialogRenameFile::getNewFilename()
{
    QString returnedName;
    returnedName=this->ui->lineEdit->text();
    return returnedName;
}

DialogRenameFile::~DialogRenameFile()
{
    delete ui;
}


