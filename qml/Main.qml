import QtQuick
import QtQuick.Controls

import "areas"
import "components"

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: qsTr("Alkali")

    Rectangle {
        anchors.fill: parent
        color: "#222"

        SplitView {
            id: splitView

            anchors.fill: parent
            anchors.margins: 5
            anchors.bottomMargin: 0
            orientation: Qt.Horizontal

            Area {
                label: "Tools"
            }
            SplitView {
                orientation: Qt.Vertical
                SplitView.fillWidth: true
                Area {
                    label: "Viewer"

                    ImageViewer {
                        id: viewerItem
                        objectName: "viewer"
                        anchors.fill: parent
                    }
                }
                Area {
                    label: "Network Editor"
                }
            }
            Area {
                label: "Knob Editor"
            }
        }
    }

    footer: StatusBar {}
}
