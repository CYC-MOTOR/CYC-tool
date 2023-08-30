import QtQuick 2.7
 
import QtQuick.Layouts 1.3
 

Item {
    id: mainItem
    anchors.fill: parent
    anchors.margins: 20
   
    ColumnLayout {
        anchors.fill: parent
           
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "EBMX X-9000 Firmware"
                       

            color: "pink"
             wrapMode: Text.WordWrap
        }
        
        
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "Note: This firmware is dedicated to EBMX X-9000 Controller, please read carefully before performing any actions"
                       

            color: "white"
             wrapMode: Text.WordWrap
        }
        
        
         Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "1. Please do not use Profiles, BMS, APP CFG, Packages tabs on this app"                      
            color: "white"
             wrapMode: Text.WordWrap
        }
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "2. Important parameters like current limit, voltage limit are locked and not affected by the MOTOR CFG tab"                      
            color: "white"
             wrapMode: Text.WordWrap
        }
        
        
         Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
          //  id: txt
            text: "Firmware Update Procedure"
            color: "green"
        }
        
        
        
         
         Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "1. Go to Firmware tab, swipe to Bootloader page, choose 'generic' hardware and click 'UPLOAD'  "                      
            color: "white"
             wrapMode: Text.WordWrap
        }
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
            text: "2. Swipe to Custom File, browse your firmware file from official EBMX support, click 'UPLOAD'"                      
            color: "white"
             wrapMode: Text.WordWrap
        }
        
        
        
        /*
        Text {
            Layout.fillWidth: true
            Layout.fillHeight: true
          //  id: txt
            text: "* "
            color: "red"
        }*/
         
        
         
        
    }
     
    
}
