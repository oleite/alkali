import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ToolBar {
    height: 20

    background: Rectangle {
        color: "#222"
    }

    RowLayout {
        anchors.fill: parent
        anchors.leftMargin: 7
        anchors.rightMargin: 7
        spacing: 15

        Label {
            font.pixelSize: 12
            text: "Waiting for input"
        }
        Item {
            Layout.fillWidth: true
        }
        Label {
            font.pixelSize: 12
            text: (activityProgress.value * 100).toString() + "%"
        }
        ProgressBar {
            id: activityProgress

            Layout.alignment: Qt.AlignVCenter
            implicitWidth: 100
            value: 0.67
        }
        Label {
            font.pixelSize: 12
            text: "A, B, C"
        }
    }
}
