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

#include "pagewelcome.h"
#include "ui_pagewelcome.h"
#include "setupwizardmotor.h"
#include "setupwizardapp.h"
#include "widgets/detectallfocdialog.h"

#include <QMessageBox>
#include <QQmlEngine>
#include <QQmlContext>
#include <QSettings>
#include <QmlHighlighter>
#include <QQuickItem>

PageWelcome::PageWelcome(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageWelcome)
{
    ui->setupUi(this);

    QString theme = Utility::getThemePath();
    ui->autoConnectButton->setIcon(QIcon());
   // ui->wizardFocSimpleButton->setIcon(QIcon(theme + "icons/Wizard-96.png"));
   // ui->wizardAppButton->setIcon(QIcon(theme + "icons/Wizard-96.png"));
   // ui->nrfPairButton->setIcon(QIcon(theme + "icons/icons8-fantasy-96.png"));
   // ui->multiSettingButton->setIcon(QIcon(theme + "icons/Settings-96.png"));
   // ui->invertDirButton->setIcon(QIcon(theme + "icons/Process-96.png"));

    layout()->setContentsMargins(0, 0, 0, 0);
    mVesc = nullptr;
    ui->bgWidget->setPixmap(QPixmap());

  /*  connect(ui->wizardFocSimpleButton, &QPushButton::clicked, [this]() {
        QMetaObject::invokeMethod(ui->qmlWidget->rootObject(), "setupMotors");
    });

    connect(ui->wizardAppButton, SIGNAL(clicked(bool)),
            this, SLOT(startSetupWizardApp()));

    connect(ui->multiSettingButton, &QPushButton::clicked, [this]() {
        QMetaObject::invokeMethod(ui->qmlWidget->rootObject(), "openMultiSettings");
    });

    connect(ui->invertDirButton, &QPushButton::clicked, [this]() {
        QMetaObject::invokeMethod(ui->qmlWidget->rootObject(), "dirSetup");
    });*/
}

PageWelcome::~PageWelcome()
{
    delete ui;
}

void PageWelcome::startSetupWizardFocSimple()
{
    if (mVesc) {
        DetectAllFocDialog::showDialog(mVesc, this);
    }
}

void PageWelcome::startSetupWizardFocQml()
{
    if (mVesc) {
        mQmlUi.startCustomGui(mVesc, "qrc:/res/qml/SetupMotorWindow.qml");
    }
}

void PageWelcome::startSetupWizardMotor()
{
    if (mVesc) {
        SetupWizardMotor w(mVesc, this);
        w.exec();
    }
}

void PageWelcome::startSetupWizardApp()
{
    if (mVesc) {
        SetupWizardApp w(mVesc, this);
        w.exec();
    }
}

VescInterface *PageWelcome::vesc() const
{
    return mVesc;
}

void PageWelcome::setVesc(VescInterface *vesc)
{
    mVesc = vesc;
}

void PageWelcome::on_autoConnectButton_clicked()
{
    Utility::autoconnectBlockingWithProgress(mVesc, this);
}

void PageWelcome::on_nrfPairButton_clicked()
{

}
