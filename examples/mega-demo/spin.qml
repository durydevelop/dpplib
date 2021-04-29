import QtQuick 2.0
import QtQuick.Controls 2.13

Rectangle {
    id: mqldefinition
    color: "green"
    width: 100
    height: 100

    Text {
        id: text
        text: "This is a text!"
        font.pointSize: 14
        anchors.centerIn: parent
    }

    BusyIndicator {
        id: busyIndicator
        x: 2
        y: 4
        wheelEnabled: false
        running: true
        focusPolicy: Qt.NoFocus
    }
}
