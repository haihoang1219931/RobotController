#include "ApplicationController.h"
#include "Button.h"
#include "Robot.h"
#include "CommandReader.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
ApplicationController::ApplicationController()
{
    for(int i=0;i< MAX_BUTTON; i++) {
        m_buttonList[i] = new Button(this);
    }
    m_commandReader = new CommandReader(this);
    m_robot = new Robot(this,3);
    m_appTimer = 0;
    m_motorScale[0] = 1.0f*1.0f;
    m_motorScale[1] =  1.0f*1.0f;
    m_motorScale[2] =  1.0f*1.0f;
    m_armLength[0] = 47 + 228;
    m_armLength[1] = 55;
    m_armLength[2] = 50;
    m_armLength[3] = 84;
    m_armLength[4] = 22+14;
    m_upAngle = 0;
    m_chessBoardPosX = 0;
    m_chessBoardPosY = 0;
    m_chessBoardSize = 32*8;
}

ApplicationController::~ApplicationController() {

}

void ApplicationController::loop() {
    m_appTimer++;
    checkAllButtonState();
    switch(m_machineState) {
    case MACHINE_WAIT_COMMAND: {
        m_commandReader->loop();
        break;
    }
    case MACHINE_EXECUTE_COMMAND: {
        m_robot->loop();
        break;
    }
    case MACHINE_EXECUTE_COMMAND_DONE: {
        this->printf("_%04d DON",m_comCommandID);
        setMachineState(MACHINE_WAIT_COMMAND);
        break;
    }
    }
}

void ApplicationController::checkAllButtonState() {
    for(int i=0;i< MAX_BUTTON; i++) {
        m_buttonList[i]->checkState();
    }
}

//int ApplicationController::buttonState(BUTTON_ID buttonID) {
//    return m_listButton[buttonID]->buttonState();
//}

MACHINE_STATE ApplicationController::stateMachine() {
    return m_machineState;
}

void ApplicationController::getCurrentPosition(float* listAngles, int* numMotor)
{
    if(m_robot == NULL) return;
    int listCurrentStep[MAX_MOTOR];
    m_robot->getCurrentPosition(listCurrentStep,numMotor);
    for(int i = 0; i < *numMotor; i++) {
        listAngles[i] = (int)((float)listCurrentStep[i]/m_motorScale[i]);
    }
}

void ApplicationController::getCurrentArmLength(float* listCurrentArmLength, int* numArm)
{
    if(m_robot == NULL) return;
    *numArm = 5;
    for(int i = 0; i < *numArm; i++) {
        listCurrentArmLength[i] = m_armLength[i];
    }
}

void ApplicationController::getChessBoardParams(float* listParam, int* numParam)
{
    *numParam = 3;
    listParam[0] = m_chessBoardPosX;
    listParam[1] = m_chessBoardPosY;
    listParam[2] = m_chessBoardSize;
}

void ApplicationController::setMachineState(MACHINE_STATE machineState) {
    if(machineState != m_machineState) {
        m_machineState = machineState;
        this->printf("APP STATE: %d\r\n",m_machineState);
    }
}

void ApplicationController::executeCommand(char* command) {
    this->printf("Command: [%s]\r\n",command);
    if(command[0] == '_') {
        if(strlen(command)>=5){
            char commandID[8];
            commandID[0] = command[1];
            commandID[1] = command[2];
            commandID[2] = command[3];
            commandID[3] = command[4];
            commandID[4] = 0;
            int comCommandID = atoi(commandID); // command from PC, must response
            if(m_comCommandID > comCommandID) {
                this->printf("_%04d NOK",comCommandID);
            } else {
                m_comCommandID = comCommandID;
                this->printf("_%04d ACK",comCommandID);
            }
        }
    }
    else if(command[0] == 'e') {
        this->enableEngine(true);
    }
    else if(command[0] == 'd') {
        this->enableEngine(false);
    }
    else if(command[0] == 'h') {
        setMachineState(MACHINE_EXECUTE_COMMAND);
        m_robot->goHome();
    }
    else if(command[0] == 't' &&
            command[1] == '1' ) {
        float angles[3] = {37,93,113};
        int steps[3] = {0,0,0};
        for(int i=0; i< 3; i++){
            steps[i] = (int)(angles[i]*m_motorScale[i]);
        }
        setMachineState(MACHINE_EXECUTE_COMMAND);
        m_robot->goToPosition(steps,2);
    }
    else if(command[0] == 't' &&
            command[1] == '2' ) {
        int steps[3] = {0,0,0};
        setMachineState(MACHINE_EXECUTE_COMMAND);
        m_robot->goToPosition(steps,2);
    }
    else if(command[0] == 't' &&
            command[1] == '3' ) {
        executeSequence(0,0,0,0,true,false,'c');
    }
    else if(command[0] == 't' &&
            command[1] == '4' ) {
        executeSequence(7,7,0,0,true,false,'c');
    }
    //  else if(command[0] == 'a') {
    //    if(strlen(command)<21) {
    //      this->printf("All Params: a [base] [arm1] [arm2] [servo]\r\n");
    //      return;
    //    }
    //    char angleStr[16];
    //    angleStr[0] = command[2];
    //    angleStr[1] = command[3];
    //    angleStr[2] = command[4];
    //    angleStr[3] = command[5];
    //    angleStr[4] = 0;
    //    long angleBase = atoi(angleStr);

    //    angleStr[0] = command[7];
    //    angleStr[1] = command[8];
    //    angleStr[2] = command[9];
    //    angleStr[3] = command[10];
    //    angleStr[4] = 0;
    //    long angleArm1 = atoi(angleStr);

    //    angleStr[0] = command[12];
    //    angleStr[1] = command[13];
    //    angleStr[2] = command[14];
    //    angleStr[3] = command[15];
    //    angleStr[4] = 0;
    //    long angleArm2 = atoi(angleStr);

    //    angleStr[0] = command[17];
    //    angleStr[1] = command[18];
    //    angleStr[2] = command[19];
    //    angleStr[3] = command[20];
    //    angleStr[4] = 0;
    //    long angleStepper = atoi(angleStr);
    //    m_robot->ablsoluteAngle(angleBase,angleArm1,angleArm2,angleStepper);
    //  }
    //    else if(command[0] == 'p'){
    //    if(strlen(command)<16) {
    //      this->printf("Mission Params: p [startCol] [startRow] [stopCol] [stopRow] [a/n][c/n][promote piece]\r\n");
    //      return;
    //    }
    //    int startCol = (command[2]-'0')*10+(command[3]-'0');
    //    int startRow = (command[5]-'0')*10+(command[6]-'0');
    //    int stopCol = (command[8]-'0')*10+(command[9]-'0');
    //    int stopRow = (command[11]-'0')*10+(command[12]-'0');
    //    bool attack = command[13] == 'a';
    //    bool castle = command[14] == 'c';
    //    char promote = command[15];
    //    m_robot->goToPosition(startCol, startRow, stopCol, stopRow, attack, castle, promote);
    //  } else  if(command[0] == 'r') {
    //    if(strlen(command)<12) {
    //      this->printf("Mission Params: r[motor:0-3] [position:0000-9999] [speed:0000-9999]\r\n");
    //      return;
    //    }
    //    int motorID = command[1]-'0';
    //    char angleStr[16];
    //    angleStr[0] = command[3];
    //    angleStr[1] = command[4];
    //    angleStr[2] = command[5];
    //    angleStr[3] = command[6];
    //    angleStr[4] = 0;
    //    long angle = atoi(angleStr);

    //    char speedStr[16];
    //    speedStr[0] = command[8];
    //    speedStr[1] = command[9];
    //    speedStr[2] = command[10];
    //    speedStr[3] = command[11];
    //    speedStr[4] = 0;
    //    float speed = atof(speedStr);
    //    m_robot->rotateAngle((MOTOR)motorID, angle, speed);
    //  }
}
void ApplicationController::calculateRobotArm(float x, float y, float a1, float a2, float* p1, float* p2)
{
  *p2 = acos((x*x+y*y-a1*a1-a2*a2)/(2*a1*a2));
  *p1 = atan(y/x) - atan((a2*sin(*p2))/(a1+a2*cos(*p2)));
  *p1 =  *p1 < 0?*p1+PI:*p1;
  this->printf("x[%d] y[%d] a1[%d] a2[%d] q1[%d] q2[%d]\r\n",
                (int)x,(int)y,(int)a1,(int)a2,
               (int)(*p1/M_PI*180.0f),(int)(*p2/M_PI*180.0f));
}

void ApplicationController::executeSequence(
        int startCol, int startRow,
        int stopCol, int stopRow,
        bool attack, bool castle, char promote) {
  this->printf("Go to Pos [%d,%d] to [%d,%d] attack[%s] castle[%s] promote[%c]\r\n",
                  startCol, startRow, stopCol, stopRow, attack?"yes":"no", castle?"yes":"no", promote);
  int centerCol = 0;
  int centerRow = 0;
  float squareLength = m_chessBoardSize/8;
  float captureAngles[8] = {0.0f,0.0f,2000.0f,2000.0f,2000.0f,2000.0f,0.0f,0.0f};
  float upAngles[8] = {55.0f,55.0f,55.0f,100.0f,100.0f,55.0f,55.0f,100.0f};
  int positionRow[8] = {startRow,startRow,startRow,startRow,stopRow,stopRow,stopRow,stopRow};
  int positionCol[8] = {startCol,startCol,startCol,startCol,stopCol,stopCol,stopCol,stopCol};
//  resetMoveSequene();
//  for(int seqStep = 0; seqStep < 8; seqStep++)
  {
    int seqStep = 0;
    float yPos = (float)(positionCol[seqStep]-centerCol) * squareLength
            + squareLength/2 + m_chessBoardPosX;
    float xPos = (float)(positionRow[seqStep]-centerRow) * squareLength
            + squareLength/2 + m_chessBoardPosY;
    float upAngle = (upAngles[seqStep] - 55.0f)/180.0f*PI;
    float a1 = m_armLength[0];
    float a2Cos = m_armLength[1];
    float a2Sin = m_armLength[2] + m_armLength[3]*cos(upAngle)+m_armLength[4];
    float a2 = (float)sqrt(a2Sin*a2Sin + a2Cos*a2Cos);
    float q2Offset = atan(a2Sin/a2Cos);
    float q1 = 0;
    float q2 = 0;
    this->printf("xPos[%d]\r\n",(int)xPos);
    this->printf("yPos[%d]\r\n",(int)yPos);
    this->printf("a1[%d]\r\n",(int)a1);
    this->printf("a2[%d]\r\n",(int)a2);
    calculateRobotArm(xPos, yPos, a1, a2, &q1, &q2);
    this->printf("arm1Angle[%d]=[%d] arm2Angle[%d]=[%d] arm3Angle[%d] q2Offset[%d]\r\n",
    (int)(q1/M_PI*180.0f),
    (int)((M_PI/2 + q1)/M_PI*180.0f),
    (int)((q2)/M_PI*180.0f),
    (int)((M_PI/2 - q2 + q2Offset)/M_PI*180.0f),
    (int)(upAngles[seqStep]),
    (int)(q2Offset/M_PI*180.0f));
    int armAngle[MAX_MOTOR] = {
      (int)((M_PI/2 + q1)/M_PI*180.0f*m_motorScale[0]),
      (int)((M_PI/2 - q2 + q2Offset)/M_PI*180.0f*m_motorScale[1]),
      (int)(upAngles[seqStep]*m_motorScale[2]),
      (int)(captureAngles[seqStep])};
    m_robot->goToPosition(armAngle,2);
//    appendMoveSequence(armAngle,MOTOR::MOTOR_MAX);
  }

  setMachineState(MACHINE_EXECUTE_COMMAND);
}
