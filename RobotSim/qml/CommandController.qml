import QtQuick 2.0
import QtQml 2.0
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4
Item {
    id: root
    property alias arm1Length: cmd1.armLength
    property alias arm2Length: cmd2.armLength
    property alias arm3Length: cmd3.armLength
    property alias arm4Length: cmd4.armLength
    property alias arm5Length: cmd5.armLength

    property alias angle1: cmd1.angle
    property alias angle2: cmd2.angle
    property alias angle3: cmd3.angle
    property alias angle4: cmd4.angle
    property alias angle5: cmd5.angle
    property alias upAngle: sldArmAngle.value
    property alias chessBoardPosX: spbChessBoardPosX.value
    property alias chessBoardPosY: spbChessBoardPosY.value
    property alias chessBoardWidth: spbChessBoardWidth.value
    property alias showChessBoard: cbxChessBoard.checked

    signal pathClearClicked()

    Column {
        spacing: 10
        Row {
            spacing: 10
            CheckBox {
                id: cbxChessBoard
                width: 200
                height: 40
                text: "Chess Board"
                checked: true
            }
            Button {
                width: 100
                height: 40
                text: "Clear path"
                onClicked: {
                    root.pathClearClicked()
                }
            }
        }

        Row {
            spacing: 10
            Button {
                width: 100
                height: 40
                text: "Start"
                onClicked: {
                    mainProcess.startService();
                }
            }

            Button {
                width: 100
                height: 40
                text: running?"Pause":"Go"
                property bool running: false
                onClicked: {
                    mainProcess.pause(!running);
                    running = !running;
                }
            }

            Slider {
                id: spbRunSpeed
                width: 150
                height: 40
                from: 1
                to: 500
                stepSize: 1
                value: 500
                onValueChanged: {
                    mainProcess.changeSleepTime(value);
                }
            }
            Label {
                width: 40
                height: 40
                text: spbRunSpeed.value
                verticalAlignment: Text.AlignVCenter
            }
        }

        Row {
            spacing: 10
            Button {
                width: 100
                height: 40
                text: "Home"
                onClicked: {
                    mainProcess.executeCommand("h");
                }
            }
            Button {
                width: 40
                height: 40
                text: "T1"
                onClicked: {
                    mainProcess.executeCommand("t1");
                }
            }
            Button {
                width: 40
                height: 40
                text: "T2"
                onClicked: {
                    mainProcess.executeCommand("t2");
                }
            }
            GridView {
                id: view
                width: 320
                height: 320
                clip: true
                model: 64
                cellWidth: 40
                cellHeight: 40
                delegate: Button {
                    width: 40
                    height: 40
                    text: "C"+ (index-index%8)/8+""+index%8
                    onClicked: {
                        mainProcess.executeCommand(text.toLowerCase());
                    }
                }
            }
        }

        Row {
            spacing: 10
            Label {
                width: 100
                height: 40
                text: "ChessBoard Size"
                verticalAlignment: Text.AlignVCenter
            }
            SpinBox {
                id: spbChessBoardWidth
                from: 0
                to: 500
            }
        }
        Row {
            spacing: 10
            Label {
                width: 100
                height: 40
                text: "ChessBoard offset"
                verticalAlignment: Text.AlignVCenter
            }
            SpinBox {
                id: spbChessBoardPosX
                from: -100
                to: 100
            }
            SpinBox {
                id: spbChessBoardPosY
                from: 0
                to: 100
            }
        }
        Command {
            id: cmd1
            title: "Arm1"
        }
        Command {
            id: cmd2
            title: "Arm2"
        }
        Command {
            id: cmd3
            title: "Arm3"
            editAngle: false
        }
        Command {
            id: cmd4
            title: "Arm4"
            editAngle: false
        }
        Command {
            id: cmd5
            title: "Arm5"
            editAngle: false
        }
        Row {
            spacing: 10
            Label {
                width: 100
                height: 40
                text: "Up Angle"
                verticalAlignment: Text.AlignVCenter
            }
            Slider {
                id: sldArmAngle
                width: 300
                height: 40
                from: 0
                to: 45
            }
            Label {
                width: 40
                height: 40
                text: Number(sldArmAngle.value).toFixed(2)
                verticalAlignment: Text.AlignVCenter
            }
        }

    }
}
