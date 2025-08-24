import QtQuick 2.0

Item {
    id: item
    property real angle1: 0
    property real angle2: 0
    property real angle3: 0
    property real angle4: 0
    property real angle5: 0
    property int arm1Width: 300
    property int arm2Width: 200
    property int arm3Width: 200
    property int arm4Width: 200
    property int arm5Width: 200
    property real grabberAngle: 0

    RobotArm {
        width: arm1Width
        armAngle: angle1
        borderColor: "blue"
        RobotArm {
            width: arm2Width
            armAngle: angle2
            anchors.left: parent.right
            borderColor: "white"
            RobotArm {
                width: arm3Width
                armAngle: angle3
                borderColor: "white"
                anchors.left: parent.right
                RobotArm {
                    width: arm4Width
                    armAngle: angle4
                    borderColor: "green"
                    anchors.left: parent.right
                    RobotArm {
                        width: arm5Width
                        armAngle: angle5
                        borderColor: "green"
                        anchors.left: parent.right
                    }
                }
            }
        }
    }
}
