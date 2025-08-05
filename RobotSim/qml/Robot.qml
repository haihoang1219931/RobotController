import QtQuick 2.0

Item {
    id: item
    property real angle1: 0
    property real angle2: 0
    property real angle3: 0
    property real angle4: 0
    property int arm1Width: 300
    property int arm2Width: 200
    property int arm3Width: 200
    property int arm4Width: 200
    property real grabberAngle: 0
    RobotArm {
        width: arm1Width
        armAngle: angle1
        borderColor: "blue"
        RobotArm {
            width: arm2Width
            armAngle: angle2
            anchors.left: parent.right
            anchors.leftMargin: -height
            borderColor: "white"
            RobotArm {
                width: arm3Width
                armAngle: angle3
                borderColor: "green"
                anchors.left: parent.right
                anchors.leftMargin: -height
                Grabber {
                    width: arm4Width
                    armAngle: angle4
                    grabberAngle: item.grabberAngle
                    borderColor: "black"
                    anchors.left: parent.right
                    anchors.leftMargin: -height
                }
            }
        }
    }

    Timer {
        running: true
        interval: 30
        repeat: true
        onTriggered: {
//            angle1 ++
//            angle2 ++
//            angle4 ++
        }
    }
}
