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

#ifndef PARAMEDITDOUBLE_H
#define PARAMEDITDOUBLE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include "configparams.h"
#include "displaypercentage.h"

namespace Ui {
class ParamEditDouble;
}

class ParamEditDouble : public QWidget
{
    Q_OBJECT

public:
    explicit ParamEditDouble(QWidget *parent = 0);
    ~ParamEditDouble();
    void setConfig(ConfigParams *config);
    QString name() const;
    void setName(const QString &name);
    void setSuffix(const QString &suffix);
    void setDecimals(int decimals);
    void setShowAsPercentage(bool showAsPercentage);
    void showDisplay(bool show);

private slots:
    void paramChangedDouble(QObject *src, QString name, double newParam);
    void percentageChanged(int p);
    void doubleChanged(double d);

    void on_readButton_clicked();
    void on_readDefaultButton_clicked();
    void on_helpButton_clicked();

private:
    Ui::ParamEditDouble *ui;
    ConfigParams *mConfig;
    ConfigParam mParam;
    QString mName;
    double mMaxVal;

    DisplayPercentage *mDisplay;
    QDoubleSpinBox *mDoubleBox;
    QSpinBox *mPercentageBox;

    void updateDisplay(double val);

};

#endif // PARAMEDITDOUBLE_H
