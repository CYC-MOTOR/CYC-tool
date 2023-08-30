/*
    Copyright 2016 - 2017 Benjamin Vedder	benjamin@vedder.se

    This file is part of VESC Tool.

    VESC Tool is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    VESC Tool is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

#ifndef PAGEAPPNUNCHUK_H
#define PAGEAPPNUNCHUK_H

#include <QWidget>
#include "vescinterface.h"

namespace Ui {
class PageAppNunchuk;
}

class PageAppNunchuk : public QWidget
{
    Q_OBJECT

public:
    explicit PageAppNunchuk(QWidget *parent = nullptr);
    ~PageAppNunchuk();

    VescInterface *vesc() const;
    void setVesc(VescInterface *vesc);
    void reloadParams();

private slots:
    void decodedChukReceived(double value);
    void paramChangedDouble(QObject *src, QString name, double newParam);
    void paramChangedEnum(QObject *src, QString name, int newParam);

private:
    Ui::PageAppNunchuk *ui;
    VescInterface *mVesc;

};

#endif // PAGEAPPNUNCHUK_H
