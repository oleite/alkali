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
            anchors.margins: 5
            orientation: Qt.Horizontal

            FilledArea {
                id: toolsArea

                SplitView.preferredWidth: 50
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

                        Button {
                            Layout.fillWidth: true
                            text: "Open"

                            onClicked: fileDialog.open()
                        }
                        Button {
                            icon.color: "transparent"
                            icon.source: "assets/sipi_mandril.png"

                            onClicked: processor.source = icon.source
                        }
                        Button {
                            icon.color: "transparent"
                            icon.source: "assets/rgbtest.png"

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

                    FilledArea {
                        SplitView.preferredHeight: 300
                        clip: true
                        label: "Viewer"

                        ImageViewer {
                            id: viewerItem

                            anchors.fill: parent
                            image: processor.output
                        }
                    }
                    FilledArea {
                        label: "Network Editor"
                    }
                }
            }
            FilledArea {
                SplitView.preferredWidth: 400
                label: "Knob Editor"

                RowLayout {
                    anchors.fill: parent

                    Slider {
                        Layout.alignment: Qt.AlignTop
                        Layout.fillWidth: true
                        from: 0
                        to: 3
                        value: processor.intensity

                        onMoved: processor.intensity = value
                    }
                }
            }
        }
    }
}
