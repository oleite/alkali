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
            text: "Waiting for input"
            font.pixelSize: 12
        }

        Item {
            Layout.fillWidth: true
        }

        Label {
            text: (activityProgress.value * 100).toString() + "%"
            font.pixelSize: 12
        }

        ProgressBar {
            id: activityProgress
            value: 0.67
            implicitWidth: 100
            Layout.alignment: Qt.AlignVCenter
        }

        Label {
            text: "A, B, C"
            font.pixelSize: 12
        }
    }
}
