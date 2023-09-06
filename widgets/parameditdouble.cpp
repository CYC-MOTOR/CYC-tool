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

#include "parameditdouble.h"
#include "ui_parameditdouble.h"
#include <QMessageBox>
#include "helpdialog.h"
#include <cmath>
#include "utility.h"

ParamEditDouble::ParamEditDouble(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParamEditDouble)
{
    ui->setupUi(this);

    QString theme = Utility::getThemePath();
    ui->helpButton->setIcon(QPixmap(theme + "icons/Help-96.png"));
    ui->readButton->setIcon(QPixmap(theme + "icons/Upload-96.png"));
    ui->readDefaultButton->setIcon(QPixmap(theme + "icons/Data Backup-96.png"));

    mConfig = 0;

    min = 0;
    max = 0;
    steps = 100;

    mDoubleBox = new QDoubleSpinBox(this);
    mSlider = new QSlider(Qt::Horizontal, this);

    ui->mainLayout->insertWidget(0, mSlider);
    ui->mainLayout->insertWidget(0, mDoubleBox);
    

    ui->mainLayout->setStretchFactor(mDoubleBox, 1);
    ui->mainLayout->setStretchFactor(mSlider, 10);

    connect(mDoubleBox, SIGNAL(valueChanged(double)),
            this, SLOT(doubleChanged(double)));
    connect(mSlider, SIGNAL(valueChanged(int)),
            this, SLOT(percentageChanged(int)));
}

ParamEditDouble::~ParamEditDouble()
{
    delete ui;
}

void ParamEditDouble::setConfig(ConfigParams *config)
{
    mConfig = config;

    ConfigParam *param = mConfig->getParam(mName);

    if (param) {
        ui->readButton->setVisible(param->transmittable);
        ui->readDefaultButton->setVisible(param->transmittable);

        mParam = *param;

        min = mParam.minDouble;
        max = mParam.maxDouble;
        steps = (max - min) * mParam.editorScale * qPow(10, mDoubleBox->decimals());

        mDoubleBox->setMaximum(mParam.maxDouble * mParam.editorScale);
        mDoubleBox->setMinimum(mParam.minDouble * mParam.editorScale);
        mDoubleBox->setSingleStep(mParam.stepDouble);
        mDoubleBox->setValue(mParam.valDouble * mParam.editorScale);

        
        mSlider->setMaximum(steps);
        mSlider->setMinimum(0);
        mSlider->setSingleStep(1);
        mSlider->setValue(doubleToPercentage(mParam.valDouble));
    }

    connect(mConfig, SIGNAL(paramChangedDouble(QObject*,QString,double)),
            this, SLOT(paramChangedDouble(QObject*,QString,double)));
}

QString ParamEditDouble::name() const
{
    return mName;
}

void ParamEditDouble::setName(const QString &name)
{
    mName = name;
}

void ParamEditDouble::setSuffix(const QString &suffix)
{
    mDoubleBox->setSuffix(suffix);
}

void ParamEditDouble::setDecimals(int decimals)
{
    mDoubleBox->setDecimals(decimals);
}

void ParamEditDouble::setShowAsPercentage(bool showAsPercentage)
{
    mDoubleBox->setVisible(true);
}

void ParamEditDouble::showDisplay(bool show)
{
    
}

void ParamEditDouble::paramChangedDouble(QObject *src, QString name, double newParam)
{
    if (src != this && name == mName) {
        mDoubleBox->setValue(newParam * mParam.editorScale);
        mSlider->setValue(doubleToPercentage(newParam));
    }
}

void ParamEditDouble::percentageChanged(int p)
{
    double val = percentageToDouble(p);

    if (mConfig) {
        if (mConfig->getUpdateOnly() != mName) {
            mConfig->setUpdateOnly("");
        }
        mConfig->updateParamDouble(mName, val, this);
    }
    //qDebug() << "Step: " << p << ", " << val * mParam.editorScale;
    mDoubleBox->setValue(val * mParam.editorScale);
}

void ParamEditDouble::doubleChanged(double d)
{
    double val = d / mParam.editorScale;

    if (mConfig) {
        if (mConfig->getUpdateOnly() != mName) {
            mConfig->setUpdateOnly("");
        }
        mConfig->updateParamDouble(mName, val, this);
    }
    //qDebug() << d << ", Step: " << doubleToPercentage(val);
    mSlider->setValue(doubleToPercentage(val));
}

void ParamEditDouble::on_readButton_clicked()
{
    if (mConfig) {
        mConfig->setUpdateOnly(mName);
        mConfig->requestUpdate();
    }
}

void ParamEditDouble::on_readDefaultButton_clicked()
{
    if (mConfig) {
        mConfig->setUpdateOnly(mName);
        mConfig->requestUpdateDefault();
    }
}

void ParamEditDouble::on_helpButton_clicked()
{
    if (mConfig) {
        HelpDialog::showHelp(this, mConfig, mName);
    }
}

int ParamEditDouble::doubleToPercentage(double val)
{
    double total = max - min;
    double newVal = min < 0 ? val + fabs(min) : val - min;
    return round(newVal / total * steps);
}

double ParamEditDouble::percentageToDouble(int p)
{
    double total = max - min;
    return min + total * ((double)p / steps);
}
