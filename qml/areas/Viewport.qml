import QtQuick

Rectangle {
    id: viewport

    property alias image: renderItem.image
    property alias imageHeight: renderItem.imageHeight
    property alias imageWidth: renderItem.imageWidth
    property int marginTop: 12
    property int padding: 10
    property real panX: 0
    property real panY: 0
    property real zoom: 1

    function fitToView() {
        const w = width - padding * 2;
        const h = height - padding * 2 - marginTop * 2;

        if (renderItem.width <= 0 || renderItem.height <= 0 || w <= 0 || h <= 0) {
            zoom = 1;
        } else {
            const sx = w / renderItem.width;
            const sy = h / renderItem.height;
            zoom = Math.min(sx, sy);
        }

        panX = 0;
        panY = marginTop;
    }

    clip: true
    color: "#1C1E21"
    focus: true

    Keys.onPressed: function (event) {
        if (event.key === Qt.Key_F) {
            fitToView();
            event.accepted = true;
        }
    }

    WheelHandler {
        acceptedDevices: PointerDevice.TouchPad
        target: null

        onWheel: event => {
            if (event.phase === Qt.ScrollMomentum) {
                return;
            }

            if (event.pixelDelta.x !== 0 || event.pixelDelta.y !== 0) {
                viewport.panX += event.pixelDelta.x;
                viewport.panY += event.pixelDelta.y;
            } else {
                viewport.panX += event.angleDelta.x * 0.5;
                viewport.panY += event.angleDelta.y * 0.5;
            }
        }
    }
    PinchHandler {
        id: pinch

        target: null

        onScaleChanged: delta => {
            var dx = pinch.centroid.position.x - viewport.width / 2 - viewport.panX;
            var dy = pinch.centroid.position.y - viewport.height / 2 - viewport.panY;
            viewport.panX -= dx * (delta - 1);
            viewport.panY -= dy * (delta - 1);
            viewport.zoom *= delta;
        }
    }
    Item {
        id: content

        height: renderItem.height
        scale: viewport.zoom
        transformOrigin: Item.Center
        width: renderItem.width
        x: (viewport.width - width) / 2 + viewport.panX
        y: (viewport.height - height) / 2 + viewport.panY

        ImageRenderItem {
            id: renderItem

            anchors.centerIn: parent
            height: imageHeight
            width: imageWidth

            onImageChanged: {
                Qt.callLater(viewport.fitToView);
            }
        }
    }
}
