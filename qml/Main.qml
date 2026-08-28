pragma ComponentBehavior: Bound

import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs
import QtQuick.Layouts

import "areas"
import "components"

ApplicationWindow {
    color: "#181818"
    height: 720
    title: qsTr("Alkali")
    visible: true
    width: 1080

    footer: StatusBar {
    }

    Processor {
        id: processor

        source: "assets/rgbtest.png"
    }
    FileDialog {
        id: fileDialog

        currentFolder: StandardPaths.standardLocations(StandardPaths.PicturesLocation)[0]

        onAccepted: processor.source = selectedFile
    }
    AlkSplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        FilledArea {
            id: toolsArea

            SplitView.preferredWidth: 70
            clip: true
            label: "Tools"

            ToolBar {
                anchors.fill: parent

                background: Rectangle {
                    color: "transparent"
                    radius: toolsArea.radius
                }

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 5

                    ToolButton {
                        Layout.fillWidth: true
                        text: "Open.."

                        onClicked: fileDialog.open()
                    }
                    Repeater {
                        model: ["assets/andromeda.png", "assets/house.tiff", "assets/rgbtest.png", "assets/serpa.jpeg", "assets/sipi_mandril.png", "assets/woodpecker.jpg"]

                        delegate: ImageSourceButton {
                            required property string modelData

                            imageSource: modelData
                        }
                    }
                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
        Area {
            SplitView.fillWidth: true

            AlkSplitView {
                anchors.fill: parent
                orientation: Qt.Vertical

                ViewerArea {
                    SplitView.preferredHeight: 350
                    image: processor.output
                }
                FilledArea {
                    label: "Network Editor"
                }
            }
        }
        FilledArea {
            SplitView.preferredWidth: 400
            label: "Knob Editor"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 5

                Item {
                    Layout.preferredHeight: 30
                }
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 30
                    color: "#444"
                    radius: 5

                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 5
                        spacing: 10

                        Label {
                            text: "Exposure"
                        }
                        Slider {
                            id: exposureSlider

                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            from: 0
                            to: 3
                            value: processor.intensity

                            onMoved: processor.intensity = value
                        }
                        Label {
                            text: exposureSlider.value.toFixed(2)
                        }
                    }
                }
                Item {
                    Layout.fillHeight: true
                }
            }
        }
    }

    component ImageSourceButton: ToolButton {
        id: button

        required property url imageSource

        Layout.fillWidth: true
        Layout.preferredHeight: 40
        icon.color: "transparent"
        icon.source: imageSource

        contentItem: Image {
            anchors.fill: parent
            anchors.margins: 5
            fillMode: Image.PreserveAspectCrop
            source: button.imageSource
        }

        onClicked: processor.source = imageSource
    }
}
