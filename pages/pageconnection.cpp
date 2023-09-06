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

#include "pageconnection.h"
#include "ui_pageconnection.h"
#include "widgets/helpdialog.h"
#include "utility.h"
#include <QMessageBox>
#include <QListWidgetItem>
#include <QInputDialog>

PageConnection::PageConnection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PageConnection)
{
    ui->setupUi(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    mVesc = nullptr;
    mTimer = new QTimer(this);

    connect(mTimer, SIGNAL(timeout()),
            this, SLOT(timerSlot()));

    mTimer->start(20);

    QString theme = Utility::getThemePath();
    ui->canRefreshButton->setIcon(QIcon());
    ui->serialRefreshButton->setIcon(QIcon());
    ui->serialConnectButton->setIcon(QIcon());
    ui->serialDisconnectButton->setIcon(QIcon());
    ui->canDefaultButton->setIcon(QIcon());
    ui->helpButton->setIcon(QPixmap(theme + "icons/Help-96.png"));
    ui->autoConnectButton->setIcon(QIcon());

    QIcon mycon = QIcon(theme + "icons/can_off.png");
    mycon.addPixmap(QPixmap(theme + "icons/can_off.png"), QIcon::Normal, QIcon::Off);
    mycon.addPixmap(QPixmap(theme + "icons/can_on.png"), QIcon::Normal, QIcon::On);
    ui->canFwdButton->setIcon(mycon);
}

PageConnection::~PageConnection()
{
    delete ui;
}

VescInterface *PageConnection::vesc() const
{
    return mVesc;
}

void PageConnection::setVesc(VescInterface *vesc)
{
    mVesc = vesc;

#ifdef HAS_BLUETOOTH
    connect(mVesc->bleDevice(), SIGNAL(scanDone(QVariantMap,bool)),
            this, SLOT(bleScanDone(QVariantMap,bool)));

    QString lastBleAddr = mVesc->getLastBleAddr();
    if (lastBleAddr != "") {
        QString setName = mVesc->getBleName(lastBleAddr);

        QString name;
        if (!setName.isEmpty()) {
            name += setName;
            name += " [";
            name += lastBleAddr;
            name += "]";
        } else {
            name = lastBleAddr;
        }
        ui->bleDevBox->insertItem(0, name, lastBleAddr);
    }
#endif

#ifdef HAS_SERIALPORT
    ui->serialBaudBox->setValue(mVesc->getLastSerialBaud());
#endif

#ifdef HAS_CANBUS
    ui->CANbusBitrateBox->setValue(mVesc->getLastCANbusBitrate());

    ui->CANbusInterfaceBox->clear();
    QList<QString> interfaces = mVesc->listCANbusInterfaces();

    for(int i = 0;i < interfaces.size();i++) {
        ui->CANbusInterfaceBox->addItem(interfaces.at(i), interfaces.at(i));
    }

    ui->CANbusInterfaceBox->setCurrentIndex(0);
#endif

    connect(mVesc->commands(), SIGNAL(pingCanRx(QVector<int>,bool)),
            this, SLOT(pingCanRx(QVector<int>,bool)));
    connect(mVesc, SIGNAL(CANbusNewNode(int)),
            this, SLOT(CANbusNewNode(int)));
    connect(mVesc, SIGNAL(CANbusInterfaceListUpdated()),
            this, SLOT(CANbusInterfaceListUpdated()));
    on_serialRefreshButton_clicked();
}

void PageConnection::pingCanRx(QVector<int> devs, bool isTimeout)
{
    (void)isTimeout;
    ui->canRefreshButton->setEnabled(true);

    ui->canFwdBox->clear();
    for (int dev: devs) {
        ui->canFwdBox->addItem(QString("VESC %1").arg(dev), dev);
    }
}

void PageConnection::CANbusNewNode(int node)
{

}

void PageConnection::CANbusInterfaceListUpdated()
{

}

void PageConnection::on_serialRefreshButton_clicked()
{
    if (mVesc) {
        ui->serialPortBox->clear();
        QList<VSerialInfo_t> ports = mVesc->listSerialPorts();
        foreach(const VSerialInfo_t &port, ports) {
            ui->serialPortBox->addItem(port.name, port.systemPath);
        }
        ui->serialPortBox->setCurrentIndex(0);
    }
}

void PageConnection::on_serialDisconnectButton_clicked()
{
    if (mVesc) {
        mVesc->disconnectPort();
    }
}

void PageConnection::on_serialConnectButton_clicked()
{
    if (mVesc) {
        mVesc->connectSerial(ui->serialPortBox->currentData().toString(),
                             ui->serialBaudBox->value());
    }
}

void PageConnection::on_CANbusScanButton_clicked()
{

}

void PageConnection::on_CANbusDisconnectButton_clicked()
{
    if (mVesc) {
        mVesc->disconnectPort();
    }
}

void PageConnection::on_CANbusConnectButton_clicked()
{

}

void PageConnection::on_helpButton_clicked()
{
    if (mVesc) {
        HelpDialog::showHelp(this, mVesc->infoConfig(), "help_can_forward");
    }
}

void PageConnection::on_canFwdButton_toggled(bool checked)
{
    if (mVesc) {
        if (mVesc->commands()->getCanSendId() >= 0 || !checked) {
            mVesc->commands()->setSendCan(checked);
        } else {
            mVesc->emitMessageDialog("CAN Forward",
                                     "No CAN device is selected. Click on the refresh button "
                                     "if the selection box is empty.",
                                     false, false);
        }
    }
}

void PageConnection::on_autoConnectButton_clicked()
{
    Utility::autoconnectBlockingWithProgress(mVesc, this);
}

void PageConnection::on_canFwdBox_currentIndexChanged(const QString &arg1)
{
    (void)arg1;
    if (mVesc && ui->canFwdBox->count() > 0) {
        mVesc->commands()->setCanSendId(quint32(ui->canFwdBox->currentData().toInt()));
    }
}

void PageConnection::on_canRefreshButton_clicked()
{
    if (mVesc) {
        ui->canRefreshButton->setEnabled(false);
        mVesc->commands()->pingCan();
    }
}

void PageConnection::on_canDefaultButton_clicked()
{
    ui->canFwdBox->clear();
    for (int dev = 0;dev < 255;dev++) {
        ui->canFwdBox->addItem(QString("VESC %1").arg(dev), dev);
    }
}

void PageConnection::on_pairConnectedButton_clicked()
{
    if (mVesc) {
        if (mVesc->isPortConnected()) {
            if (mVesc->commands()->isLimitedMode()) {
                mVesc->emitMessageDialog("Pair VESC",
                                         "The fiwmare must be updated to pair this VESC.",
                                         false, false);
            } else {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::warning(this,
                                             tr("Pair connected VESC"),
                                             tr("This is going to pair the connected VESC with this instance of VESC Tool. VESC Tool instances "
                                                "that are not paired with this VESC will not be able to connect over bluetooth any more. Continue?"),
                                             QMessageBox::Ok | QMessageBox::Cancel);
                if (reply == QMessageBox::Ok) {
                    mVesc->addPairedUuid(mVesc->getConnectedUuid());
                    mVesc->storeSettings();
                    ConfigParams *ap = mVesc->appConfig();
                    mVesc->commands()->getAppConf();
                    bool res = Utility::waitSignal(ap, SIGNAL(updated()), 1500);

                    if (res) {
                        mVesc->appConfig()->updateParamBool("pairing_done", true, nullptr);
                        mVesc->commands()->setAppConf();
                    }
                }
            }
        } else {
            mVesc->emitMessageDialog("Pair VESC",
                                     "You are not connected to the VESC. Connect in order to pair it.",
                                     false, false);
        }
    }
}

void PageConnection::on_addConnectedButton_clicked()
{
    if (mVesc) {
        if (mVesc->isPortConnected()) {
            mVesc->addPairedUuid(mVesc->getConnectedUuid());
            mVesc->storeSettings();
        } else {
            mVesc->emitMessageDialog("Add UUID",
                                     "You are not connected to the VESC. Connect in order to add it.",
                                     false, false);
        }
    }
}

void PageConnection::on_addUuidButton_clicked()
{
    if (mVesc) {
        bool ok;
        QString text = QInputDialog::getText(this, "Add UUID",
                                             "UUID:", QLineEdit::Normal,
                                             "", &ok);
        if (ok) {
            mVesc->addPairedUuid(text);
            mVesc->storeSettings();
        }
    }
}
