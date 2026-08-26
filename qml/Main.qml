import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import "areas"
import "components"

ApplicationWindow {
    width: 1080
    height: 720
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
                SplitView.preferredWidth: 50
            }
            SplitView {
                orientation: Qt.Vertical
                SplitView.fillWidth: true
                Area {
                    label: "Viewer"
                    clip: true
                    SplitView.preferredHeight: 300

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
                SplitView.preferredWidth: 400

                RowLayout {
                    anchors.fill: parent
                    Slider {
                        id: sliderItem
                        objectName: "slider"

                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignTop
                    }
                }
            }
        }
    }

    footer: StatusBar {}
}
