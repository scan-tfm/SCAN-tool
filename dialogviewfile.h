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

#ifndef DIALOGVIEWFILE_H
#define DIALOGVIEWFILE_H

#include <QDialog>

namespace Ui {
class DialogViewfile;
}

class DialogViewfile : public QDialog
{
    Q_OBJECT

public:
    explicit DialogViewfile(QString fileText, QWidget *parent = 0);
    ~DialogViewfile();

private:
    Ui::DialogViewfile *ui;
};

#endif // DIALOGVIEWFILE_H
