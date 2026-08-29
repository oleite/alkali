import QtQuick

Rectangle {
    id: viewport

    property alias image: renderItem.image
    property alias imageHeight: renderItem.imageHeight
    property alias imageWidth: renderItem.imageWidth
    property real maxZoom: 100.0
    property real minZoom: 0.001
    property real padBottom: 10
    property real padLeft: 10
    property real padRight: 10
    property real padTop: 35
    property vector2d pan: Qt.vector2d(0, 0)
    property real zoom: 1
    property real zoomDragSensitivity: 0.005
    property real zoomWheelSensitivity: 2.0

    function clampZoom(value) {
        return Math.max(minZoom, Math.min(value, maxZoom));
    }
    function fitToView() {
        const imageWidth = renderItem.imageWidth;
        const imageHeight = renderItem.imageHeight;

        const availableWidth = width - padLeft - padRight;
        const availableHeight = height - padTop - padBottom;

        if (imageWidth <= 0 || imageHeight <= 0 || availableWidth <= 0 || availableHeight <= 0) {
            zoom = 1;
            pan = Qt.vector2d(0, 0);
            return;
        }

        const sx = availableWidth / imageWidth;
        const sy = availableHeight / imageHeight;

        zoom = clampZoom(Math.min(sx, sy));

        pan = Qt.vector2d((padLeft - padRight) / 2, (padTop - padBottom) / 2);
    }
    function zoomAround(origin, factor) {
        if (!Number.isFinite(factor) || factor <= 0) {
            return;
        }

        const previousZoom = zoom;
        const newZoom = clampZoom(previousZoom * factor);
        const scaleDelta = newZoom / previousZoom;

        const originFromCenter = Qt.vector2d(origin.x - width / 2, origin.y - height / 2);

        const positionDelta = originFromCenter.minus(pan);

        pan = pan.minus(positionDelta.times(scaleDelta - 1));
        zoom = newZoom;
    }

    clip: true
    color: "#1C1E21"
    focus: true

    Component.onCompleted: {
        Qt.callLater(viewport.fitToView);
    }

    /* Keyboard */

    Keys.onPressed: function (event) {
        if (event.key === Qt.Key_F) {
            fitToView();
            event.accepted = true;
        }
    }

    /* Focus */

    TapHandler {
        acceptedButtons: Qt.LeftButton | Qt.MiddleButton | Qt.RightButton

        onPressedChanged: {
            if (pressed) {
                viewport.forceActiveFocus();
            }
        }
    }

    /* Mouse */

    // Pan (MMB)
    DragHandler {
        acceptedButtons: Qt.MiddleButton
        acceptedModifiers: Qt.NoModifier
        target: null

        onActiveChanged: {
            if (active) {
                viewport.forceActiveFocus();
            }
        }
        xAxis.onActiveValueChanged: delta => {
            viewport.pan = viewport.pan.plus(Qt.vector2d(delta, 0));
        }
        yAxis.onActiveValueChanged: delta => {
            viewport.pan = viewport.pan.plus(Qt.vector2d(0, delta));
        }
    }

    // Pan (Alt+LMB)
    DragHandler {
        acceptedButtons: Qt.LeftButton
        acceptedModifiers: Qt.AltModifier
        target: null

        onActiveChanged: {
            if (active) {
                viewport.forceActiveFocus();
            }
        }
        xAxis.onActiveValueChanged: delta => {
            viewport.pan = viewport.pan.plus(Qt.vector2d(delta, 0));
        }
        yAxis.onActiveValueChanged: delta => {
            viewport.pan = viewport.pan.plus(Qt.vector2d(0, delta));
        }
    }

    // Zoom (Alt+RMB)
    DragHandler {
        function applyZoomDelta(delta) {
            const factor = Math.exp(delta * viewport.zoomDragSensitivity);

            viewport.zoomAround(centroid.pressPosition, factor);
        }

        acceptedButtons: Qt.RightButton
        acceptedModifiers: Qt.AltModifier
        target: null

        onActiveChanged: {
            if (active) {
                viewport.forceActiveFocus();
            }
        }
        xAxis.onActiveValueChanged: delta => {
            applyZoomDelta(delta);
        }
        yAxis.onActiveValueChanged: delta => {
            applyZoomDelta(delta);
        }
    }

    // Zoom (Wheel)
    WheelHandler {
        acceptedDevices: PointerDevice.Mouse
        acceptedModifiers: Qt.NoModifier
        target: null

        onActiveChanged: {
            if (active) {
                viewport.forceActiveFocus();
            }
        }
        onWheel: event => {
            if (event.phase === Qt.ScrollMomentum) {
                return;
            }

            let factor = 1;
            if (event.pixelDelta.y !== 0) {
                factor = Math.exp(event.pixelDelta.y * 0.005 * viewport.zoomWheelSensitivity);
            } else if (event.angleDelta.y !== 0) {
                const steps = event.angleDelta.y / 120;
                factor = Math.pow(1.1, steps * viewport.zoomWheelSensitivity);
            }

            viewport.zoomAround(point.position, factor);
        }
    }

    /* Touchpad */

    // Pan
    WheelHandler {
        acceptedDevices: PointerDevice.TouchPad
        acceptedModifiers: Qt.NoModifier
        target: null

        onActiveChanged: {
            if (active) {
                viewport.forceActiveFocus();
            }
        }
        onWheel: event => {
            if (event.phase === Qt.ScrollMomentum) {
                return;
            }

            if (event.pixelDelta.x !== 0 || event.pixelDelta.y !== 0) {
                const delta = Qt.vector2d(event.pixelDelta.x, event.pixelDelta.y);

                viewport.pan = viewport.pan.plus(delta);
            } else {
                const delta = Qt.vector2d(event.angleDelta.x, event.angleDelta.y).times(0.5);

                viewport.pan = viewport.pan.plus(delta);
            }
        }
    }

    // Zoom (Pinch)
    PinchHandler {
        acceptedModifiers: Qt.NoModifier
        target: null

        onActiveChanged: {
            if (active) {
                viewport.forceActiveFocus();
            }
        }
        onScaleChanged: delta => {
            viewport.zoomAround(centroid.position, delta);
        }
    }

    /* Image */

    Item {
        id: content

        height: renderItem.height
        scale: viewport.zoom
        transformOrigin: Item.Center
        width: renderItem.width
        x: (viewport.width - width) / 2 + viewport.pan.x
        y: (viewport.height - height) / 2 + viewport.pan.y

        ImageRenderItem {
            id: renderItem

            anchors.centerIn: parent
            height: imageHeight
            width: imageWidth

            onImageChanged: viewport.fitToView()
        }
    }
}
