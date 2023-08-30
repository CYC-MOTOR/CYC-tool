/*
    Copyright 2016 - 2021 Benjamin Vedder	benjamin@vedder.se

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

#include "adcmap.h"
#include "ui_adcmap.h"
#include "helpdialog.h"
#include "utility.h"
#include <QMessageBox>

AdcMap::AdcMap(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AdcMap)
{
    ui->setupUi(this);

    QString theme = Utility::getThemePath();
    ui->helpButton->setIcon(QPixmap(theme + "icons/Help-96.png"));
    ui->resetButton->setIcon(QPixmap(theme + "icons/Restart-96.png"));
    ui->applyButton->setIcon(QPixmap(theme + "icons/apply.png"));

    layout()->setContentsMargins(0, 0, 0, 0);
    mVesc = 0;
    mResetDone = true;
    on_controlTypeBox_currentIndexChanged(ui->controlTypeBox->currentIndex());
}

AdcMap::~AdcMap()
{
    delete ui;
}

VescInterface *AdcMap::vesc() const
{
    return mVesc;
}

void AdcMap::setVesc(VescInterface *vesc)
{
    mVesc = vesc;

    if (mVesc) {
        ConfigParam *p = mVesc->appConfig()->getParam("app_adc_conf.ctrl_type");
        if (p) {
            ui->controlTypeBox->addItems(p->enumNames);
        }

        connect(mVesc->commands(), SIGNAL(decodedAdcReceived(double,double,double,double)),
                this, SLOT(decodedAdcReceived(double,double,double,double)));
    }
}

void AdcMap::decodedAdcReceived(double value, double voltage, double value2, double voltage2)
{
    if (ui->displayCh1->isDual()) {
        double p = (value - 0.5) * 200.0;

        ui->displayCh1->setValue(p);
        ui->displayCh1->setText(tr("%1 V (%2 %)").
                                arg(voltage, 0, 'f', 2).
                                arg(p, 0, 'f', 1));
    } else {
        double p = value * 100.0;

        ui->displayCh1->setValue(p);
        ui->displayCh1->setText(tr("%1 V (%2 %)").
                                arg(voltage, 0, 'f', 4).
                                arg(p, 0, 'f', 1));
    }

    if (ui->displayCh2->isDual()) {
        double p = (value2 - 0.5) * 200.0;

        ui->displayCh2->setValue(p);
        ui->displayCh2->setText(tr("%1 V (%2 %)").
                                arg(voltage2, 0, 'f', 4).
                                arg(p, 0, 'f', 1));
    } else {
        double p = value2 * 100.0;

        ui->displayCh2->setValue(p);
        ui->displayCh2->setText(tr("%1 V (%2 %)").
                                arg(voltage2, 0, 'f', 4).
                                arg(p, 0, 'f', 1));
    }

    if (mResetDone) {
        mResetDone = false;
        ui->minCh1Box->setValue(voltage);
        ui->maxCh1Box->setValue(voltage);
        ui->minCh2Box->setValue(voltage2);
        ui->maxCh2Box->setValue(voltage2);
    } else {
        if (voltage < ui->minCh1Box->value()) {
            ui->minCh1Box->setValue(voltage);
        }

        if (voltage > ui->maxCh1Box->value()) {
            ui->maxCh1Box->setValue(voltage);
        }

        if (voltage2 < ui->minCh2Box->value()) {
            ui->minCh2Box->setValue(voltage2);
        }

        if (voltage2 > ui->maxCh2Box->value()) {
            ui->maxCh2Box->setValue(voltage2);
        }
    }

    double range = ui->maxCh1Box->value() - ui->minCh1Box->value();
    double pos = voltage - ui->minCh1Box->value();

    if (pos > (range / 4.0) && pos < ((3.0 * range) / 4.0)) {
        ui->centerCh1Box->setValue(voltage);
    } else {
        ui->centerCh1Box->setValue(range / 2.0 + ui->minCh1Box->value());
    }
}

void AdcMap::on_controlTypeBox_currentIndexChanged(int index)
{
    // Cast to the enum to get a warning when unhandled cases
    // are added in datatypes.h.
    switch (adc_control_type(index)) {
    case ADC_CTRL_TYPE_CURRENT_REV_CENTER:
    case ADC_CTRL_TYPE_CURRENT_NOREV_BRAKE_CENTER:
    case ADC_CTRL_TYPE_DUTY_REV_CENTER:
    case ADC_CTRL_TYPE_PID_REV_CENTER:
    case ADC_CTRL_TYPE_CURRENT_REV_BUTTON_BRAKE_CENTER:
        ui->displayCh1->setDual(true);
        break;

    case ADC_CTRL_TYPE_NONE:
    case ADC_CTRL_TYPE_CURRENT:
    case ADC_CTRL_TYPE_CURRENT_REV_BUTTON:
    case ADC_CTRL_TYPE_CURRENT_NOREV_BRAKE_BUTTON:
    case ADC_CTRL_TYPE_CURRENT_NOREV_BRAKE_ADC:
    case ADC_CTRL_TYPE_DUTY:
    case ADC_CTRL_TYPE_DUTY_REV_BUTTON:
    case ADC_CTRL_TYPE_PID_REV_BUTTON:
    case ADC_CTRL_TYPE_CURRENT_REV_BUTTON_BRAKE_ADC:
    case ADC_CTRL_TYPE_PID:
        ui->displayCh1->setDual(false);
        break;
    }

    ui->displayCh1->setEnabled(index != ADC_CTRL_TYPE_NONE);
    ui->displayCh2->setEnabled(index == ADC_CTRL_TYPE_CURRENT_NOREV_BRAKE_ADC ||
                               index == ADC_CTRL_TYPE_CURRENT_REV_BUTTON_BRAKE_ADC);
}

void AdcMap::on_helpButton_clicked()
{
    if (mVesc) {
        HelpDialog::showHelp(this, mVesc->infoConfig(), "app_adc_mapping_help");
    }
}

void AdcMap::on_resetButton_clicked()
{
    mResetDone = true;
    ui->minCh1Box->setValue(0.0);
    ui->maxCh1Box->setValue(0.0);
    ui->minCh2Box->setValue(0.0);
    ui->maxCh2Box->setValue(0.0);
}

void AdcMap::on_applyButton_clicked()
{
    if (mVesc) {
        if (ui->maxCh1Box->value() > 1e-10) {
            mVesc->appConfig()->updateParamDouble("app_adc_conf.voltage_start", ui->minCh1Box->value());
            mVesc->appConfig()->updateParamDouble("app_adc_conf.voltage_end", ui->maxCh1Box->value());
            mVesc->appConfig()->updateParamDouble("app_adc_conf.voltage_center", ui->centerCh1Box->value());
            mVesc->appConfig()->updateParamDouble("app_adc_conf.voltage2_start", ui->minCh2Box->value());
            mVesc->appConfig()->updateParamDouble("app_adc_conf.voltage2_end", ui->maxCh2Box->value());
            mVesc->emitStatusMessage(tr("Start, End and Center ADC Voltages Applied"), true);
        } else {
            mVesc->emitStatusMessage(tr("Applying Pulselengths Failed"), false);
            QMessageBox::warning(this,
                                 tr("Apply Voltages"),
                                 tr("Please activate RT app data and measure the ADC voltages first."));
        }
    }
}
