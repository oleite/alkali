import QtQuick
import QtQuick.Controls

Rectangle {
    id: area

    property string label: "Area"

    SplitView.preferredWidth: 100
    SplitView.preferredHeight: 100
    SplitView.minimumWidth: 30
    SplitView.minimumHeight: 30

    color: "lightcoral"
    radius: 7

    Label {
        text: area.label
        anchors.centerIn: parent
    }
}