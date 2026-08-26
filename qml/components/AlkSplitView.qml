import QtQuick
import QtQuick.Controls

SplitView {
    handle: Item {
        id: handle

        readonly property real gripThickness: 12
        readonly property bool vertical: width < height

        implicitHeight: 4
        implicitWidth: 4

        containmentMask: Item {
            height: Math.max(handle.height, handle.gripThickness)
            width: Math.max(handle.width, handle.gripThickness)
            x: (handle.width - width) / 2
            y: (handle.height - height) / 2
        }

        // Workaround for Qt 6.11.1:
        // events outside the handle (containmentMask) arent being
        // delivered unless we explicitly define this MouseArea
        MouseArea {
            anchors.centerIn: parent
            cursorShape: handle.vertical ? Qt.SplitHCursor : Qt.SplitVCursor
            height: Math.max(handle.height, handle.gripThickness)
            hoverEnabled: true
            width: Math.max(handle.width, handle.gripThickness)
        }
        Rectangle {
            anchors.centerIn: parent
            color: "#111"
            height: handle.vertical ? handle.height - 4 : 6
            opacity: handle.SplitHandle.pressed ? 1 : 0
            radius: 3
            visible: opacity > 0
            width: handle.vertical ? 6 : handle.width - 4

            Behavior on opacity {
                NumberAnimation {
                    duration: 100
                    easing.type: Easing.InOutQuad
                }
            }

            Rectangle {
                anchors.fill: parent
                anchors.margins: 2
                color: "#888"
                radius: 1
            }
        }
    }
}
