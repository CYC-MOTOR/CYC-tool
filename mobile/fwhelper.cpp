/*
    Copyright 2017 - 2020 Benjamin Vedder	benjamin@vedder.se

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

#include "fwhelper.h"
#include <QDirIterator>
#include "hexfile.h"

FwHelper::FwHelper(QObject *parent) : QObject(parent)
{

}

QVariantMap FwHelper::getHardwares(FW_RX_PARAMS params, QString hw)
{
    QVariantMap hws;

    QString fwDir = "://res/firmwares";

    if (params.hwType == HW_TYPE_VESC_BMS) {
        fwDir = "://res/firmwares_bms";
    }

    QDirIterator it(fwDir);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        QStringList names = fi.fileName().split("_o_");

        if (fi.isDir() && (hw.isEmpty() || names.contains(hw, Qt::CaseInsensitive))) {
            QString name = names.at(0);
            for(int i = 1;i < names.size();i++) {
                name += " & " + names.at(i);
            }

            hws.insert(name, fi.absoluteFilePath());
        }
    }

    return hws;
}

QVariantMap FwHelper::getFirmwares(QString hw)
{
    QVariantMap fws;

    QDirIterator it(hw);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        fws.insert(fi.fileName(), fi.absoluteFilePath());
    }

    return fws;
}

QVariantMap FwHelper::getBootloaders(FW_RX_PARAMS params, QString hw)
{
    QVariantMap bls;

    QString blDir = "://res/bootloaders";

    if (params.hwType == HW_TYPE_VESC_BMS) {
        blDir = "://res/bootloaders_bms";
    }

    QDirIterator it(blDir);
    while (it.hasNext()) {
        QFileInfo fi(it.next());
        QStringList names = fi.fileName().replace(".bin", "").split("_o_");

        if (!fi.isDir() && (hw.isEmpty() || names.contains(hw, Qt::CaseInsensitive))) {
            QString name = names.at(0);
            for(int i = 1;i < names.size();i++) {
                name += " & " + names.at(i);
            }

            bls.insert(name, fi.absoluteFilePath());
        }
    }

    if (bls.isEmpty()) {
        QFileInfo generic(blDir + "/generic.bin");
        if (generic.exists()) {
            bls.insert("generic", generic.absoluteFilePath());
        }
    }

    return bls;
}

bool FwHelper::uploadFirmware(QString filename, VescInterface *vesc,
                              bool isBootloader, bool checkName, bool fwdCan)
{
    if (filename.startsWith("file:/")) {
        filename.remove(0, 6);
    }

    QFile file;
    file.setFileName(filename);
    QFileInfo fileInfo(filename);

    if (checkName) {
        if (!fileInfo.fileName().toLower().endsWith(".bin") &&
                !fileInfo.fileName().toLower().endsWith(".hex")) {
            vesc->emitMessageDialog(
                        tr("Upload Error"),
                        tr("The selected file name seems to be invalid."),
                        false, false);
            return false;
        }
    }

    if (!file.open(QIODevice::ReadOnly)) {
        vesc->emitMessageDialog(tr("Upload Error"),
                                tr("Could not open file. Make sure that the path is valid."),
                                false);
        qDebug() << fileInfo.fileName() << fileInfo.absolutePath();
        return false;
    }

    if (file.size() > 400000) {
        vesc->emitMessageDialog(tr("Upload Error"),
                                tr("The selected file is too large to be a firmware."),
                                false);
        return false;
    }

    bool fwRes = false;

    if (file.fileName().toLower().endsWith(".hex")) {
        QMap<quint32, QByteArray> fwData;
        fwRes = HexFile::parseFile(file.fileName(), fwData);

        if (fwRes) {
            QMapIterator<quint32, QByteArray> i(fwData);

            QByteArray data;
            bool startSet = false;
            unsigned int startOffset = 0;

            while (i.hasNext()) {
                i.next();
                if (!startSet) {
                    startSet = true;
                    startOffset = i.key();
                }

                while ((quint32(data.size()) + startOffset) < i.key()) {
                    data.append(char(0xFF));
                }

                data.append(i.value());
            }

            fwRes = vesc->fwUpload(data, isBootloader, fwdCan);
        }
    } else {
        QByteArray data = file.readAll();
        fwRes = vesc->fwUpload(data, isBootloader, fwdCan);
    }

    return fwRes;
}

bool FwHelper::uploadFirmwareSingleShotTimer(QString filename, VescInterface *vesc,
                              bool isBootloader, bool checkName, bool fwdCan, QString BLfilename)
{
    bool res;
    QTimer::singleShot(10, [this, &res, filename, vesc, isBootloader, checkName, fwdCan, BLfilename]() {
        qDebug() << filename;
        if(BLfilename.isEmpty()) {
            res = uploadFirmware(filename, vesc, isBootloader, checkName, fwdCan);
        } else {
            res = uploadFirmware(BLfilename, vesc, true, checkName, fwdCan);
            if(res) {
                res = uploadFirmware(filename, vesc, isBootloader, checkName, fwdCan);
            }
        }
        emit fwUploadRes(res, isBootloader);
    });
    return true;
}
