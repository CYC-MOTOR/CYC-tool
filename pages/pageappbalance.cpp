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

#include "pageappbalance.h"
#include "ui_pageappbalance.h"
#include "utility.h"

PageAppBalance::PageAppBalance(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageAppBalance)
{
    ui->setupUi(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    mVesc = nullptr;

    mTimer = new QTimer(this);
    mTimer->start(20);
    mUpdatePlots = false;
    mSecondCounter = 0.0;
    mLastUpdateTime = 0;
    mAppADC1 = 0.0;
    mAppADC2 = 0.0;

    connect(mTimer, SIGNAL(timeout()),
            this, SLOT(timerSlot()));

    ui->balancePlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    int graphIndex = 0;

    Utility::setPlotColors(ui->balancePlot);
    ui->balancePlot->addGraph();
    ui->balancePlot->graph(graphIndex)->setPen(QPen(Utility::getAppQColor("plot_graph1")));
    ui->balancePlot->graph(graphIndex)->setName("PID Output");
    graphIndex++;

    ui->balancePlot->addGraph();
    ui->balancePlot->graph(graphIndex)->setPen(QPen(Utility::getAppQColor("plot_graph2")));
    ui->balancePlot->graph(graphIndex)->setName("Pitch Angle");
    graphIndex++;

    ui->balancePlot->addGraph();
    ui->balancePlot->graph(graphIndex)->setPen(QPen(Qt::cyan));
    ui->balancePlot->graph(graphIndex)->setName("Roll Angle");
    graphIndex++;

    ui->balancePlot->addGraph();
    ui->balancePlot->graph(graphIndex)->setPen(QPen(Utility::getAppQColor("plot_graph3")));
    ui->balancePlot->graph(graphIndex)->setName("Current");
    graphIndex++;

    ui->balancePlot->addGraph();
    ui->balancePlot->graph(graphIndex)->setPen(QPen(Utility::getAppQColor("plot_graph4")));
    ui->balancePlot->graph(graphIndex)->setName("Debug 1");
    ui->balancePlot->graph(graphIndex)->removeFromLegend();
    graphIndex++;

    ui->balancePlot->addGraph();
    ui->balancePlot->graph(graphIndex)->setPen(QPen(Utility::getAppQColor("plot_graph5")));
    ui->balancePlot->graph(graphIndex)->setName("Debug 2");
    ui->balancePlot->graph(graphIndex)->removeFromLegend();
    graphIndex++;

    QFont legendFont = font();
    legendFont.setPointSize(9);

    ui->balancePlot->legend->setVisible(true);
    ui->balancePlot->legend->setFont(legendFont);
    ui->balancePlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignRight|Qt::AlignBottom);
    ui->balancePlot->xAxis->setLabel("Seconds (s)");

}

PageAppBalance::~PageAppBalance()
{
    delete ui;
}

VescInterface *PageAppBalance::vesc() const
{
    return mVesc;
}

void PageAppBalance::setVesc(VescInterface *vesc)
{
    mVesc = vesc;

    if (mVesc) {
        reloadParams();

        updateTextOutput();

        connect(mVesc->commands(), SIGNAL(decodedBalanceReceived(BALANCE_VALUES)),
                this, SLOT(appValuesReceived(BALANCE_VALUES)));
    }
}

void PageAppBalance::reloadParams()
{
    if (mVesc) {
        ui->tunePane->clearParams();
        ui->modifiersPane->clearParams();
        ui->startupPane->clearParams();
        ui->tiltbackPane->clearParams();
        ui->faultPane->clearParams();
        ui->multiescPane->clearParams();

        ui->tunePane->addParamSubgroup(mVesc->appConfig(), "balance", "tune");
        ui->modifiersPane->addParamSubgroup(mVesc->appConfig(), "balance", "tune modifiers");
        ui->startupPane->addParamSubgroup(mVesc->appConfig(), "balance", "startup");
        ui->tiltbackPane->addParamSubgroup(mVesc->appConfig(), "balance", "tiltback");
        ui->faultPane->addParamSubgroup(mVesc->appConfig(), "balance", "fault");
        ui->multiescPane->addParamSubgroup(mVesc->appConfig(), "balance", "multi esc");
    }
}

void PageAppBalance::timerSlot()
{
    if (mUpdatePlots) {

        mUpdatePlots = false;

        int dataSize = mAppPidOutputVec.size();

        QVector<double> xAxis(dataSize);
        for (int i = 0;i < mSeconds.size();i++) {
            xAxis[i] = mSeconds[i];
        }

        int graphIndex = 0;
        ui->balancePlot->graph(graphIndex++)->setData(xAxis, mAppPidOutputVec);
        ui->balancePlot->graph(graphIndex++)->setData(xAxis, mAppMAngleVec);
        ui->balancePlot->graph(graphIndex++)->setData(xAxis, mAppCAngleVec);
        ui->balancePlot->graph(graphIndex++)->setData(xAxis, mAppMotorCurrentVec);
        if(mAppDebug2.size() < 2 || (mAppDebug1.rbegin()[0] == 0 && mAppDebug1.rbegin()[1] == 0)){
            ui->balancePlot->graph(graphIndex)->setVisible(false);
            ui->balancePlot->graph(graphIndex)->removeFromLegend();
        }else{
            ui->balancePlot->graph(graphIndex)->setVisible(true);
            ui->balancePlot->graph(graphIndex)->addToLegend();
        }
        ui->balancePlot->graph(graphIndex++)->setData(xAxis, mAppDebug1);
        if(mAppDebug2.size() < 2 || (mAppDebug2.rbegin()[0] == 0 && mAppDebug2.rbegin()[1] == 0)){
            ui->balancePlot->graph(graphIndex)->setVisible(false);
            ui->balancePlot->graph(graphIndex)->removeFromLegend();
        }else{
            ui->balancePlot->graph(graphIndex)->setVisible(true);
            ui->balancePlot->graph(graphIndex)->addToLegend();
        }
        ui->balancePlot->graph(graphIndex++)->setData(xAxis, mAppDebug2);

        ui->balancePlot->rescaleAxes();

        ui->balancePlot->replot();

        updateTextOutput();
    }
}

void PageAppBalance::appValuesReceived(BALANCE_VALUES values) {

    const int maxS = 250;

    appendDoubleAndTrunc(&mAppPidOutputVec, values.pid_output, maxS);
    appendDoubleAndTrunc(&mAppMAngleVec, values.pitch_angle, maxS);
    appendDoubleAndTrunc(&mAppCAngleVec, values.roll_angle, maxS);
    mAppDiffTime = values.diff_time;
    appendDoubleAndTrunc(&mAppMotorCurrentVec, values.motor_current, maxS);
    mAppState = values.state;
    mAppSwitchValue = values.switch_value;
    mAppADC1 = values.adc1;
    mAppADC2 = values.adc2;
    appendDoubleAndTrunc(&mAppDebug1, values.debug1, maxS);
    appendDoubleAndTrunc(&mAppDebug2, values.debug2, maxS);


    qint64 tNow = QDateTime::currentMSecsSinceEpoch();

    double elapsed = (double)(tNow - mLastUpdateTime) / 1000.0;
    if (elapsed > 1.0) {
        elapsed = 1.0;
    }

    mSecondCounter += elapsed;

    appendDoubleAndTrunc(&mSeconds, mSecondCounter, maxS);

    mLastUpdateTime = tNow;

    mUpdatePlots = true;
}

void PageAppBalance::appendDoubleAndTrunc(QVector<double> *vec, double num, int maxSize)
{
    vec->append(num);

    if(vec->size() > maxSize) {
        vec->remove(0, vec->size() - maxSize);
    }
}

void PageAppBalance::updateTextOutput(){
    QString output = "Loop Time: ";
    output = output + QString::number(mAppDiffTime);

    output = output + "\tState: ";
    if(mAppState == 0){
        output = output + "Calibrating";
    }else if(mAppState == 1){
        output = output + "Running";
    }else if(mAppState == 2){
        output = output + "Running (Tiltback Duty)";
    }else if(mAppState == 3){
        output = output + "Running (Tiltback High Voltage)";
    }else if(mAppState == 4){
        output = output + "Running (Tiltback Low Voltage)";
    }else if(mAppState == 5){
        output = output + "Unknown"; // Formerly Constant Tiltback, currently unused
    }else if(mAppState == 6){
        output = output + "Fault (Pitch Angle)";
    }else if(mAppState == 7){
        output = output + "Fault (Roll Angle)";
    }else if(mAppState == 8){
        output = output + "Fault (Switch Half)";
    }else if(mAppState == 9){
        output = output + "Fault (Switch FULL)";
    }else if(mAppState == 10){
        output = output + "Fault (Duty)";
    }else if(mAppState == 11){
        output = output + "Initial";
    }else{
        output = output + "Unknown";
    }

    output = output + "\tADC1: ";
    output = output + QString::number(mAppADC1, 'f', 2);
    output = output + " ADC2: ";
    output = output + QString::number(mAppADC2, 'f', 2);
    output = output + " Switch Value: ";
    if(mAppSwitchValue == 0){
        output = output + "Off";
    }else if(mAppSwitchValue == 1){
        output = output + "Half";
    }else if(mAppSwitchValue == 2){
        output = output + "On";
    }else{
        output = output + "Unknown";
    }

    ui->textOutput->setText(output);
}
