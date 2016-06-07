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

#ifndef DIALOGRENAMEFILE_H
#define DIALOGRENAMEFILE_H

#include <QDialog>

namespace Ui {
class DialogRenameFile;
}

class DialogRenameFile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRenameFile(QString inputFileName, QWidget *parent = 0);
    //returns the new file name entered in the text box
    QString getNewFilename();
    ~DialogRenameFile();

private slots:


private:
    Ui::DialogRenameFile *ui;
};

#endif // DIALOGRENAMEFILE_H
