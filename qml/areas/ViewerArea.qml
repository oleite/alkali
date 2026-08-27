import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

FilledArea {
    id: viewerArea

    property alias image: viewport.image

    label: "Viewer"

    Viewport {
        id: viewport

        anchors.fill: parent
    }
    RowLayout {
        anchors.left: parent.left
        anchors.margins: 5
        anchors.right: parent.right
        anchors.top: parent.top
        height: 17
        spacing: 5

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#444"
            radius: 5

            Label {
                anchors.centerIn: parent
                text: "ACES"
            }
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#444"
            radius: 5

            Label {
                anchors.centerIn: parent
                text: "Display - sRGB"
            }
        }
        Item {
            Layout.fillWidth: true
        }
        Item {
            Layout.fillWidth: true
        }
        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#444"
            radius: 5

            Label {
                anchors.centerIn: parent
                text: viewport.imageWidth + " x " + viewport.imageHeight
            }
        }
    }
}
