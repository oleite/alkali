import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

import "areas"
import "components"

ApplicationWindow {
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
    Rectangle {
        id: mainWrapper

        anchors.fill: parent
        color: "#181818"

        AlkSplitView {
            id: splitView

            anchors.bottomMargin: 0
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
                        ToolButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            icon.color: "transparent"
                            icon.source: "assets/andromeda.png"

                            contentItem: Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                fillMode: Image.PreserveAspectCrop
                                source: parent.icon.source
                            }

                            onClicked: processor.source = icon.source
                        }
                        ToolButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            icon.color: "transparent"
                            icon.source: "assets/house.tiff"

                            contentItem: Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                fillMode: Image.PreserveAspectCrop
                                source: parent.icon.source
                            }

                            onClicked: processor.source = icon.source
                        }
                        ToolButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            icon.color: "transparent"
                            icon.source: "assets/rgbtest.png"

                            contentItem: Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                fillMode: Image.PreserveAspectCrop
                                source: parent.icon.source
                            }

                            onClicked: processor.source = icon.source
                        }
                        ToolButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            icon.color: "transparent"
                            icon.source: "assets/serpa.jpeg"

                            contentItem: Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                fillMode: Image.PreserveAspectCrop
                                source: parent.icon.source
                            }

                            onClicked: processor.source = icon.source
                        }
                        ToolButton {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            icon.color: "transparent"
                            icon.source: "assets/sipi_mandril.png"

                            contentItem: Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                fillMode: Image.PreserveAspectCrop
                                source: parent.icon.source
                            }

                            onClicked: processor.source = icon.source
                        }
                        ToolButton {
                            id: woodpecker

                            Layout.fillWidth: true
                            Layout.preferredHeight: 40
                            icon.color: "transparent"
                            icon.source: "assets/woodpecker.jpg"

                            contentItem: Image {
                                anchors.fill: parent
                                anchors.margins: 5
                                fillMode: Image.PreserveAspectCrop
                                source: parent.icon.source
                            }

                            onClicked: processor.source = icon.source
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
                        height: 30
                    }
                    Rectangle {
                        Layout.fillWidth: true
                        color: "#444"
                        height: 30
                        radius: 5

                        RowLayout {
                            anchors.fill: parent
                            anchors.margins: 5
                            spacing: 10

                            Label {
                                text: "Exposure"
                            }
                            Slider {
                                id: slider

                                Layout.fillHeight: true
                                Layout.fillWidth: true
                                from: 0
                                to: 3
                                value: processor.intensity

                                onMoved: processor.intensity = value
                            }
                            Label {
                                text: slider.value.toFixed(2)
                            }
                        }
                    }
                    Item {
                        Layout.fillHeight: true
                    }
                }
            }
        }
    }
}
