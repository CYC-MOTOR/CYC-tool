/*
    Copyright 2016 - 2020 Benjamin Vedder	benjamin@vedder.se

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

#ifndef VESCINTERFACE_H
#define VESCINTERFACE_H

#include <QObject>
#include <QDateTime>
#include <QTimer>
#include <QByteArray>
#include <QList>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSettings>
#include <QHash>
#include <QFile>

#ifdef HAS_SERIALPORT
#include <QSerialPort>
#endif

#ifdef HAS_CANBUS
#include <QCanBus>
#endif

#include "datatypes.h"
#include "configparams.h"
#include "commands.h"
#include "packet.h"
#include "tcpserversimple.h"
#include "udpserversimple.h"

#ifdef HAS_BLUETOOTH
#include "bleuart.h"
#endif

#ifdef HAS_POS
#include <QGeoPositionInfoSource>
#endif

#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#endif

class VescInterface : public QObject
{
    Q_OBJECT
public:
    explicit VescInterface(QObject *parent = nullptr);
    ~VescInterface();
    Q_INVOKABLE Commands *commands() const;
    Q_INVOKABLE ConfigParams *mcConfig();
    Q_INVOKABLE ConfigParams *appConfig();
    Q_INVOKABLE ConfigParams *infoConfig();
    Q_INVOKABLE ConfigParams *fwConfig();
    Q_INVOKABLE QStringList getSupportedFirmwares();
    QList<QPair<int, int> > getSupportedFirmwarePairs();
    Q_INVOKABLE QString getFirmwareNow();
    QPair<int, int> getFirmwareNowPair();
    Q_INVOKABLE void emitStatusMessage(const QString &msg, bool isGood);
    Q_INVOKABLE void emitMessageDialog(const QString &title, const QString &msg, bool isGood, bool richText = false);
    Q_INVOKABLE bool fwRx();
    Q_INVOKABLE void storeSettings();

    // Profiles
    Q_INVOKABLE QVariantList getProfiles();
    Q_INVOKABLE void addProfile(QVariant profile);
    Q_INVOKABLE void clearProfiles();
    Q_INVOKABLE void deleteProfile(int index);
    Q_INVOKABLE void moveProfileUp(int index);
    Q_INVOKABLE void moveProfileDown(int index);
    Q_INVOKABLE MCCONF_TEMP getProfile(int index);
    Q_INVOKABLE void updateProfile(int index, QVariant profile);
    Q_INVOKABLE bool isProfileInUse(int index);
    Q_INVOKABLE MCCONF_TEMP createMcconfTemp();
    Q_INVOKABLE void updateMcconfFromProfile(MCCONF_TEMP profile);

    // Pairing
    Q_INVOKABLE QStringList getPairedUuids();
    Q_INVOKABLE bool addPairedUuid(QString uuid);
    Q_INVOKABLE bool deletePairedUuid(QString uuid);
    Q_INVOKABLE void clearPairedUuids();
    Q_INVOKABLE bool hasPairedUuid(QString uuid);
    Q_INVOKABLE QString getConnectedUuid();

    Q_INVOKABLE bool isIntroDone();
    Q_INVOKABLE void setIntroDone(bool done);

    Q_INVOKABLE QString getLastTcpServer() const;
    Q_INVOKABLE int getLastTcpPort() const;
    Q_INVOKABLE QString getLastUdpServer() const;
    Q_INVOKABLE int getLastUdpPort() const;
#ifdef HAS_SERIALPORT
    Q_INVOKABLE QString getLastSerialPort() const;
    Q_INVOKABLE int getLastSerialBaud() const;
#endif
#ifdef HAS_CANBUS
    Q_INVOKABLE QString getLastCANbusInterface() const;
    Q_INVOKABLE int getLastCANbusBitrate() const;
#endif

    // SWD Programming
    bool swdEraseFlash();
    bool swdUploadFw(QByteArray newFirmware, uint32_t startAddr = 0,
                     bool verify = false, bool isLzo = true);
    void swdCancel();
    bool swdReboot();

    // Firmware Updates
    bool fwEraseNewApp(bool fwdCan, quint32 fwSize);
    bool fwEraseBootloader(bool fwdCan);
    bool fwUpload(QByteArray &newFirmware, bool isBootloader = false, bool fwdCan = false, bool isLzo = true);
    Q_INVOKABLE void fwUploadCancel();
    Q_INVOKABLE double getFwUploadProgress();
    Q_INVOKABLE QString getFwUploadStatus();
    Q_INVOKABLE bool isCurrentFwBootloader();

    // Logging
    Q_INVOKABLE bool openRtLogFile(QString outDirectory);
    Q_INVOKABLE void closeRtLogFile();
    Q_INVOKABLE bool isRtLogOpen();
    Q_INVOKABLE QString rtLogFilePath();
    Q_INVOKABLE QVector<LOG_DATA> getRtLogData();
    Q_INVOKABLE bool loadRtLogFile(QString file);
    Q_INVOKABLE LOG_DATA getRtLogSample(double progress);
    Q_INVOKABLE LOG_DATA getRtLogSampleAtValTimeFromStart(int time);

    // Persistent settings
    Q_INVOKABLE bool useImperialUnits();
    Q_INVOKABLE void setUseImperialUnits(bool useImperialUnits);
    Q_INVOKABLE bool keepScreenOn();
    Q_INVOKABLE void setKeepScreenOn(bool on);
    Q_INVOKABLE bool useWakeLock();
    Q_INVOKABLE void setUseWakeLock(bool on);
    Q_INVOKABLE bool setWakeLock(bool lock);
    Q_INVOKABLE bool getLoadQmlUiOnConnect() const;
    Q_INVOKABLE void setLoadQmlUiOnConnect(bool loadQmlUiOnConnect);
    Q_INVOKABLE bool getAllowScreenRotation() const;
    Q_INVOKABLE void setAllowScreenRotation(bool allowScreenRotation);
    Q_INVOKABLE bool speedGaugeUseNegativeValues();
    Q_INVOKABLE void setSpeedGaugeUseNegativeValues(bool useNegativeValues);


#ifdef HAS_BLUETOOTH
    Q_INVOKABLE BleUart* bleDevice();
    Q_INVOKABLE void storeBleName(QString address, QString name);
    Q_INVOKABLE QString getBleName(QString address);
    Q_INVOKABLE QString getLastBleAddr() const;
    Q_INVOKABLE void storeBlePreferred(QString address, bool preferred);
    Q_INVOKABLE bool getBlePreferred(QString address);
#endif

    // Connection
    Q_INVOKABLE bool isPortConnected();
    Q_INVOKABLE void disconnectPort();
    Q_INVOKABLE bool reconnectLastPort();
    Q_INVOKABLE bool autoconnect();
    Q_INVOKABLE QString getConnectedPortName();
    bool connectSerial(QString port, int baudrate = 115200);
    QList<VSerialInfo_t> listSerialPorts();
    QList<QString> listCANbusInterfaces();
    Q_INVOKABLE bool connectCANbus(QString backend, QString ifName, int bitrate);
    Q_INVOKABLE bool isCANbusConnected();
    Q_INVOKABLE void setCANbusReceiverID(int node_ID);
    Q_INVOKABLE void scanCANbus();

    Q_INVOKABLE void connectTcp(QString server, int port);
    Q_INVOKABLE void connectUdp(QString server, int port);
    Q_INVOKABLE void connectBle(QString address);
    Q_INVOKABLE bool isAutoconnectOngoing() const;
    Q_INVOKABLE double getAutoconnectProgress() const;
    Q_INVOKABLE QVector<int> scanCan();
    Q_INVOKABLE QVector<int> getCanDevsLast() const;
    Q_INVOKABLE void ignoreCanChange(bool ignore);
    Q_INVOKABLE bool isIgnoringCanChanges();
    Q_INVOKABLE void canTmpOverride(bool fwdCan, int canId);
    Q_INVOKABLE void canTmpOverrideEnd();

    Q_INVOKABLE bool tcpServerStart(int port);
    Q_INVOKABLE void tcpServerStop();
    Q_INVOKABLE bool tcpServerIsRunning();
    Q_INVOKABLE bool tcpServerIsClientConnected();
    Q_INVOKABLE QString tcpServerClientIp();

    Q_INVOKABLE bool udpServerStart(int port);
    Q_INVOKABLE void udpServerStop();
    Q_INVOKABLE bool udpServerIsRunning();
    Q_INVOKABLE bool udpServerIsClientConnected();
    Q_INVOKABLE QString udpServerClientIp();

    Q_INVOKABLE void emitConfigurationChanged();

    Q_INVOKABLE bool getFwSupportsConfiguration() const;

    // Configuration backups
    Q_INVOKABLE bool confStoreBackup(bool can, QString name = "");
    Q_INVOKABLE bool confRestoreBackup(bool can);
    Q_INVOKABLE bool confLoadBackup(QString uuid);
    Q_INVOKABLE QStringList confListBackups();
    Q_INVOKABLE void confClearBackups();
    Q_INVOKABLE QString confBackupName(QString uuid);

    Q_INVOKABLE bool deserializeFailedSinceConnected();

    Q_INVOKABLE FW_RX_PARAMS getLastFwRxParams();

    Q_INVOKABLE int customConfigNum();
    Q_INVOKABLE bool customConfigsLoaded();
    ConfigParams *customConfig(int configNum);

    Q_INVOKABLE bool qmlHwLoaded();
    Q_INVOKABLE bool qmlAppLoaded();
    Q_INVOKABLE QString qmlHw();
    Q_INVOKABLE QString qmlApp();

signals:
    void statusMessage(const QString &msg, bool isGood);
    void messageDialog(const QString &title, const QString &msg, bool isGood, bool richText);
    void fwUploadStatus(const QString &status, double progress, bool isOngoing);
    void serialPortNotWritable(const QString &port);
    void fwRxChanged(bool rx, bool limited);
    void portConnectedChanged();
    void autoConnectProgressUpdated(double progress, bool isOngoing);
    void autoConnectFinished();
    void profilesUpdated();
    void pairingListUpdated();
    void CANbusNewNode(int node);
    void CANbusInterfaceListUpdated();
    void useImperialUnitsChanged(bool useImperialUnits);
    void configurationChanged();
    void configurationBackupsChanged();
    void customConfigLoadDone();
    void qmlLoadDone();

public slots:

private slots:
#ifdef HAS_SERIALPORT
    void serialDataAvailable();
    void serialPortError(QSerialPort::SerialPortError error);
#endif

#ifdef HAS_CANBUS
    void CANbusDataAvailable();
    void CANbusError(QCanBusDevice::CanBusError error);
#endif

    void tcpInputConnected();
    void tcpInputDisconnected();
    void tcpInputDataAvailable();
    void tcpInputError(QAbstractSocket::SocketError socketError);

    void udpInputError(QAbstractSocket::SocketError socketError);
    void udpInputDataAvailable();

#ifdef HAS_BLUETOOTH
    void bleDataRx(QByteArray data);
#endif

    void timerSlot();
    void packetDataToSend(QByteArray &data);
    void packetReceived(QByteArray &data);
    void cmdDataToSend(QByteArray &data);
    void fwVersionReceived(FW_RX_PARAMS params);
    void appconfUpdated();
    void mcconfUpdated();
    void ackReceived(QString ackType);

private:
    typedef enum {
        CONN_NONE = 0,
        CONN_SERIAL,
        CONN_CANBUS,
        CONN_TCP,
        CONN_BLE,
        CONN_UDP,
    } conn_t;

    QSettings mSettings;
    QHash<QString, QString> mBleNames;
    QHash<QString, bool> mBlePreferred;
    QHash<QString, CONFIG_BACKUP> mConfigurationBackups;
    QVariantList mProfiles;
    QStringList mPairedUuids;
    TcpServerSimple *mTcpServer;
    UdpServerSimple *mUdpServer;

    ConfigParams *mMcConfig;
    ConfigParams *mAppConfig;
    ConfigParams *mInfoConfig;
    ConfigParams *mFwConfig;
    QVector<ConfigParams*> mCustomConfigs;
    bool mCustomConfigsLoaded;

    bool mQmlHwLoaded;
    QString mQmlHw;
    bool mQmlAppLoaded;
    QString mQmlApp;

    QTimer *mTimer;
    Packet *mPacket;
    Commands *mCommands;
    bool mFwVersionReceived;
    bool mDeserialFailedMessageShown;
    int mFwRetries;
    int mFwPollCnt;
    QString mFwTxt;
    QPair<int, int> mFwPair;
    QString mHwTxt;
    QString mUuidStr;
    bool mIsUploadingFw;
    bool mIsLastFwBootloader;
    bool mFwSupportsConfiguration;

    // FW Upload
    bool mCancelSwdUpload;
    bool mCancelFwUpload;
    double mFwUploadProgress;
    QString mFwUploadStatus;
    bool mFwIsBootloader;

    // Connections
    conn_t mLastConnType;

#ifdef HAS_SERIALPORT
    QSerialPort *mSerialPort;
    QString mLastSerialPort;
    int mLastSerialBaud;
#endif

#ifdef HAS_CANBUS
    QCanBusDevice *mCanDevice;
    QString mLastCanDeviceInterface;
    int mLastCanDeviceBitrate;
    QString mLastCanBackend;
    int mLastCanDeviceID;
    QByteArray mCanRxBuffer;
    QVector<int> mCanNodesID;
    QList<QString> mCanDeviceInterfaces;
    bool mCANbusScanning;
#endif

    QTcpSocket *mTcpSocket;
    bool mTcpConnected;
    QString mLastTcpServer;
    int mLastTcpPort;

    QUdpSocket *mUdpSocket;
    bool mUdpConnected;
    QHostAddress mLastUdpServer;
    int mLastUdpPort;

#ifdef HAS_BLUETOOTH
    BleUart *mBleUart;
    QString mLastBleAddr;
#endif

#ifdef HAS_POS
    QGeoPositionInfoSource *mPosSource;
    QGeoPositionInfo mLastPos;
    QDateTime mLastPosTime;
#endif

#ifdef Q_OS_ANDROID
    QAndroidJniObject mWakeLock;
#endif
    bool mWakeLockActive;

    QFile mRtLogFile;
    QVector<LOG_DATA> mRtLogData;
    IMU_VALUES mLastImuValues;
    QDateTime mLastImuTime;
    SETUP_VALUES mLastSetupValues;
    QDateTime mLastSetupTime;

    bool mSendCanBefore = false;
    int mCanIdBefore = 0;
    bool mWasConnected;
    bool mAutoconnectOngoing;
    double mAutoconnectProgress;
    bool mIgnoreCanChange;

    bool mCanTmpFwdActive;
    bool mCanTmpFwdSendCanLast;
    int mCanTmpFwdIdLast;

    QVector<int> mCanDevsLast;

    FW_RX_PARAMS mLastFwParams;

    // Other settings
    bool mUseImperialUnits;
    bool mKeepScreenOn;
    bool mUseWakeLock;
    bool mLoadQmlUiOnConnect;
    bool mAllowScreenRotation;
    bool mSpeedGaugeUseNegativeValues;

    void updateFwRx(bool fwRx);
    void setLastConnectionType(conn_t type);

};

#endif // VESCINTERFACE_H
