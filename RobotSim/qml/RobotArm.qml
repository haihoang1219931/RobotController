import QtQuick 2.0

Item {
    id: root
    width: 200
    height: 40
    property real armAngle: 0
    property int joinRadius: 3
    property color borderColor
    Rectangle {
        id: join
        x: root.height/2 - joinRadius
        y: root.height/2 - joinRadius
        width: joinRadius*2
        height: joinRadius*2
        border.color: "red"
        border.width: 4
        color: "transparent"
        radius: height
    }
    Rectangle {
        id: arm
        width: root.width
        height: root.height
        border.color: borderColor
        border.width: 4
        color: "transparent"
        radius: height/2
    }
    transform: Rotation {
        origin.x: root.height/2;
        origin.y: root.height/2;
        angle: armAngle
    }
}
