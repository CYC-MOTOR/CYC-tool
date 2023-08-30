/*
    Copyright 2019 Benjamin Vedder	benjamin@vedder.se

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

import QtQuick 2.7
import QtQuick.Controls 2.10
import QtQuick.Layouts 1.3

import Vedder.vesc.vescinterface 1.0
import Vedder.vesc.utility 1.0

Item {
    property bool mLastDarkMode: false

    function openDialog() {
        mLastDarkMode = Utility.isDarkMode()
        dialog.open()
    }

    Dialog {
        id: dialog
        standardButtons: Dialog.Close
        modal: true
        focus: true
        title: "VESC Tool Settings"

        Overlay.modal: Rectangle {
            color: "#AA000000"
        }

        width: parent.width - 20
        closePolicy: Popup.CloseOnEscape

        x: 10
        y: Math.max((parent.height - height) / 2, 10)
        parent: ApplicationWindow.overlay

        ScrollView {
            anchors.fill: parent

            ColumnLayout {
                anchors.fill: parent

                CheckBox {
                    id: imperialBox
                    Layout.fillWidth: true
                    text: "Use Imperial Units"
                    checked: VescIf.useImperialUnits()
                }

                CheckBox {
                    id: negativeSpeedBox
                    Layout.fillWidth: true
                    text: "Use Negative Speed"
                    checked: VescIf.speedGaugeUseNegativeValues()
                }

                CheckBox {
                    id: screenOnBox
                    Layout.fillWidth: true
                    text: "Keep Screen On"
                    checked: VescIf.keepScreenOn()
                    visible: Qt.platform.os !== "ios"
                    enabled: Qt.platform.os !== "ios"
                }

                CheckBox {
                    id: screenRotBox
                    Layout.fillWidth: true
                    text: "Allow Screen Rotation"
                    checked: VescIf.getAllowScreenRotation()
                    visible: Qt.platform.os !== "ios"
                    enabled: Qt.platform.os !== "ios"
                }

                CheckBox {
                    id: qmlUiBox
                    Layout.fillWidth: true
                    text: "Load QML UI on Connect"
                    checked: VescIf.getLoadQmlUiOnConnect()
                }

                CheckBox {
                    id: darkModeBox
                    Layout.fillWidth: true
                    text: "Use Dark Mode"
                    checked: Utility.isDarkMode()
                    onCheckedChanged: {
                        Utility.setDarkMode(checked)
                    }
                }
            }
        }

        onOpened: {
            imperialBox.checked = VescIf.useImperialUnits()
            negativeSpeedBox.checked = VescIf.speedGaugeUseNegativeValues()
            screenOnBox.checked = VescIf.keepScreenOn()
            screenRotBox.checked = VescIf.getAllowScreenRotation()
            qmlUiBox.checked = VescIf.getLoadQmlUiOnConnect()
        }

        onClosed: {
            VescIf.setUseImperialUnits(imperialBox.checked)
            VescIf.setSpeedGaugeUseNegativeValues(negativeSpeedBox.checked)
            VescIf.setKeepScreenOn(screenOnBox.checked)
            VescIf.setAllowScreenRotation(screenRotBox.checked)
            VescIf.setLoadQmlUiOnConnect(qmlUiBox.checked)
            VescIf.storeSettings()

            Utility.keepScreenOn(VescIf.keepScreenOn())

            if (VescIf.useWakeLock()) {
                VescIf.setWakeLock(VescIf.isPortConnected())
            }

            if (Utility.isDarkMode() !== mLastDarkMode) {
                darkChangedDialog.open()
            }

            VescIf.commands().emitEmptySetupValues()
        }
    }

    Dialog {
        id: darkChangedDialog
        standardButtons: Dialog.Yes | Dialog.No
        modal: true
        focus: true
        width: parent.width - 20
        closePolicy: Popup.CloseOnEscape
        title: "Theme Changed"

        Overlay.modal: Rectangle {
            color: "#AA000000"
        }

        x: 10
        y: Math.max((parent.height - height) / 2, 10)
        parent: ApplicationWindow.overlay

        Text {
            id: detectRlLabel
            color: Utility.getAppHexColor("lightText")
            verticalAlignment: Text.AlignVCenter
            anchors.fill: parent
            wrapMode: Text.WordWrap
            text:
                "The theme has been changed. This requires restarting VESC Tool to take effect. " +
                "Do you want to close VESC Tool now?"
        }

        onAccepted: {
            Qt.quit()
        }
    }
}
