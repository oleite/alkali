import QtQuick
import QtQuick.Controls

Area {
    id: area

    property string label: "Area"

    color: "#282828"
    radius: 7

    Label {
        anchors.centerIn: parent
        text: area.label
    }
}
