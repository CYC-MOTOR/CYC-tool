// This example attempts to reconnect when the connection is lost and upload the motor
// configuration afterwards. It is useful during firmware development to keep EBMX Tool
// connected and the configuration updated.

import QtQuick 2.5
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Controls.Material 2.2
import Vedder.EBMX.utility 1.0
import Vedder.EBMX.commands 1.0
import Vedder.EBMX.configparams 1.0
import "qrc:/mobile"

Item {
    anchors.fill: parent
    anchors.margins: 10
    
    property Commands mCommands: EBMXIf.commands()
    property ConfigParams mMcConf: EBMXIf.mcConfig()
    property ConfigParams mAppConf: EBMXIf.appConfig()
    
    Timer {
        running: true
        repeat: true
        interval: 2000
        
        property bool wasConnected: true
                
        onTriggered: {
            if (!EBMXIf.isPortConnected()) {
                EBMXIf.reconnectLastPort()
                wasConnected = false
            } else {
                if (!wasConnected) {
                    mCommands.setMcconf(false)
                    wasConnected = true
                }
            }
        }
    }
}
