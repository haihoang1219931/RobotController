#include "ApplicationController.h"
#include "Button.h"
#include "Robot.h"
#include "ChessBoard.h"
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
    m_scale = 2;
    m_armLength[0] = 115;
    m_armLength[1] = 25;
    m_armLength[2] = 18;
    m_armLength[3] = 40;
    m_armLength[4] = 13;
    m_chessBoard = new ChessBoard(13-13*8/2,46,13);

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

void ApplicationController::getCurrentPosition(float* listAngles, int* numMotor, float* captureStep)
{
    if(m_robot == NULL) return;
    int listCurrentStep[MAX_MOTOR];
    m_robot->getCurrentPosition(listCurrentStep,numMotor,captureStep);
    for(int i = 0; i < *numMotor; i++) {
        listAngles[i] = (int)((float)listCurrentStep[i]/m_motorScale[i]);
    }
}

void ApplicationController::getCurrentArmLength(float* listCurrentArmLength, int* numArm)
{
    if(m_robot == NULL) return;
    *numArm = 5;
    for(int i = 0; i < *numArm; i++) {
        listCurrentArmLength[i] = m_armLength[i] * m_scale;
    }
}

void ApplicationController::getChessBoardParams(float* listParam, int* numParam)
{
    *numParam = 3;
    listParam[0] = m_chessBoard->getChessBoardPosX() * m_scale;
    listParam[1] = m_chessBoard->getChessBoardPosY() * m_scale;
    listParam[2] = m_chessBoard->getChessBoardSize() * m_scale;
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
        m_robot->goToPosition(steps,3);
    }
    else if(command[0] == 't' &&
            command[1] == '2' ) {
        int steps[3] = {0,0,0};
        setMachineState(MACHINE_EXECUTE_COMMAND);
        m_robot->goToPosition(steps,2);
    }
    else if(command[0] == 'c' && strlen(command)>=3) {
        executeSequence(MOVE_NORMAL, command[2]-'0',command[1]-'0',
                7,0);
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
bool ApplicationController::inverseKinematic(float x, float y, float a1, float a2, float* p1, float* p2)
{
    if(sqrtf(x*x+y*y) > fabs(a1+a2) || sqrtf(x*x+y*y) < fabs(a1-a2)) return false;
    *p2 = acos((x*x+y*y-a1*a1-a2*a2)/(2*a1*a2));
    *p1 = atan(y/x) - atan((a2*sin(*p2))/(a1+a2*cos(*p2)));
    *p1 =  *p1 < 0?*p1+M_PI:*p1;
    this->printf("x[%d] y[%d] a1[%d] a2[%d] q1[%d] q2[%d]\r\n",
                 (int)x,(int)y,(int)a1,(int)a2,
                 (int)(*p1/M_PI*180.0f),(int)(*p2/M_PI*180.0f));
    return true;
}

void ApplicationController::forwardKinematic(float a1, float a2, float p1, float p2, float* x, float* y) {
    *x = a1 * cos(p1) + a2 * cos(p1 + p2);
    *y = a1 * sin(p1) + a2 * sin(p1 + p2);
    this->printf("x[%d] y[%d] a1[%d] a2[%d] q1[%d] q2[%d]\r\n",
                 (int)*x,(int)*y,(int)a1,(int)a2,
                 (int)(p1/M_PI*180.0f),(int)(p2/M_PI*180.0f));
}


void ApplicationController::calculateJoints(float xPos, float yPos, float upAngleInDegree, int* jointSteps)
{    
    float upAngle = (upAngleInDegree)/180.0f*M_PI;
    float a1 = m_armLength[0];
    float a2Cos = m_armLength[1];
    float a2Sin = m_armLength[2] + m_armLength[3]*cos(upAngle)+m_armLength[4];
    float a2 = (float)sqrt(a2Sin*a2Sin + a2Cos*a2Cos - 2*a2Cos*a2Sin*cos(130.0f*M_PI/180.0f));
    float q2Offset = acosf((a2Sin*a2Sin + a2*a2 - a2Cos*a2Cos)/(2*a2*a2Sin));
    float q1 = 0;
    float q2 = 0;
    this->printf("xPos[%d]\r\n",(int)xPos);
    this->printf("yPos[%d]\r\n",(int)yPos);
    this->printf("a1[%d]\r\n",(int)a1);
    this->printf("a2[%d] cos[%.02f] upAngle[%.02f] upAngleInDegree[%.02f] PI[%f]\r\n",(int)a2,upAngle,cos(upAngle),upAngleInDegree,M_PI);
    inverseKinematic(xPos, yPos, a1, a2, &q1, &q2);

    float xPosFK = 0, yPosFK = 0;
    forwardKinematic(a1,a2,q1,q2,&xPosFK,&yPosFK);
    if(xPosFK*xPos < 0 || yPosFK * yPos < 0) q1 = q1 - M_PI;
    this->printf("arm1Angle[%d]=[%d] arm2Angle[%d]=[%d] arm3Angle[%d] q2Offset[%d]\r\n",
                 (int)(q1/M_PI*180.0f),
                 (int)((M_PI/2 + q1)/M_PI*180.0f),
                 (int)((q2)/M_PI*180.0f),
                 (int)((M_PI - q2 - q2Offset)/M_PI*180.0f),
                 (int)(upAngleInDegree),
                 (int)(q2Offset/M_PI*180.0f));
    jointSteps[0] = (int)((M_PI/2 + q1)/M_PI*180.0f*m_motorScale[0]);
    jointSteps[1] = (int)((M_PI - q2 - q2Offset)/M_PI*180.0f*m_motorScale[1]);
    jointSteps[2] = (int)(upAngleInDegree*m_motorScale[2]);
}

void ApplicationController::executeSequence(
        MOVE_TYPE moveType,
        int startCol, int startRow,
        int stopCol, int stopRow,
        char promotePiece) {
    this->printf("Go to Pos [%d,%d] to [%d,%d] \r\n",
                 startCol, startRow, stopCol, stopRow);

    // Attack: Move piece out -> Move attack piece -> Return to prepare
    // No attack: Move attack piece -> Return to prepare
    // Castle: Move king -> Move rook -> Return to prepare
    // Promote: Move pawn -> Move promote piece -> Return to prepare
    switch (moveType) {
    case MOVE_NORMAL:
        calculateSequenceMoveNormal(startCol, startRow, stopCol, stopRow);
        break;
    case MOVE_ATTACK:
        calculateSequenceAttack(startCol, startRow, stopCol, stopRow);
        break;
    case MOVE_PASTPAWN:
        calculateSequencePastPawn(startCol, startRow, stopCol, stopRow);
        break;
    case MOVE_CASTLE:
        calculateSequenceCastle(startCol, startRow, stopCol, stopRow);
        break;
    case MOVE_PROMOTE:
        calculateSequencePromotePiece(startCol, startRow, stopCol, stopRow, promotePiece);
        break;
    }

    if(m_machineState != MACHINE_EXECUTE_COMMAND)
        setMachineState(MACHINE_EXECUTE_COMMAND);
}
void ApplicationController::calculateSequenceMoveNormal(int startCol, int startRow,
                     int stopCol, int stopRow)
{
    // append move from start -> stop -> standy
    Point startPoint = m_chessBoard->convertPoint(startRow,startCol);
    Point stopPoint = m_chessBoard->convertPoint(stopRow,stopCol);
    clearSequenceMove();
    appendSequenceMove(startPoint, stopPoint);
    appendStandByMove();
    initSequenceMove(3);
}

void ApplicationController::calculateSequenceAttack(int startCol, int startRow,
                     int stopCol, int stopRow)
{
    // get free drop point
    // append move from stop -> drop -> start -> stop -> standby
    Point dropPoint = m_chessBoard->getFreeDropPoint();
    Point startPoint = m_chessBoard->convertPoint(startRow,startCol);
    Point stopPoint = m_chessBoard->convertPoint(stopRow,stopCol);
    clearSequenceMove();
    appendSequenceMove(stopPoint, dropPoint);
    appendSequenceMove(startPoint, stopPoint);
    appendStandByMove();
    initSequenceMove(3);
}

void ApplicationController::calculateSequencePastPawn(int startCol, int startRow,
                     int stopCol, int stopRow)
{
    // append move from attack pawn -> drop -> start -> stop -> standby
    Point dropPoint = m_chessBoard->getFreeDropPoint();
    Point pawnPoint = m_chessBoard->convertPoint(startRow,stopCol);
    Point startPoint = m_chessBoard->convertPoint(startRow,startCol);
    Point stopPoint = m_chessBoard->convertPoint(stopRow,stopCol);
    clearSequenceMove();
    appendSequenceMove(pawnPoint, dropPoint);
    appendSequenceMove(startPoint, stopPoint);
    appendStandByMove();
    initSequenceMove(3);
}

void ApplicationController::calculateSequencePromotePiece(int startCol, int startRow,
                     int stopCol, int stopRow, char promotePiece)
{
    // append move from attack piece -> drop -> promote -> stop -> start -> drop -> standby
    Point promotePiecePoint = m_chessBoard->getFreeDropPoint(promotePiece);
    Point startPoint = m_chessBoard->convertPoint(startRow,startCol);
    Point stopPoint = m_chessBoard->convertPoint(stopRow,stopCol);

    clearSequenceMove();
    if(startCol != stopCol){
        // pawn attack piece, move attack piece -> drop
        Point dropPoint = m_chessBoard->getFreeDropPoint();
        appendSequenceMove(stopPoint, dropPoint);
    }
    appendSequenceMove(promotePiecePoint, stopPoint);
    appendSequenceMove(startPoint, stopPoint);
    appendStandByMove();
    m_robot->moveSequence(3);
}

void ApplicationController::calculateSequenceCastle(int kingCol, int kingRow,
                                                    int rookCol, int rookRow)
{

    // append move king -> new point -> rook -> new point
    Point kingPoint = m_chessBoard->convertPoint(kingRow,kingCol);
    Point rookPoint = m_chessBoard->convertPoint(rookRow,rookCol);
    Point kingNewPoint;
    Point rookNewPoint;
    if(kingCol > rookCol) {
        kingNewPoint = m_chessBoard->convertPoint(kingRow,kingCol-2);
        rookNewPoint = m_chessBoard->convertPoint(kingRow,kingCol-1);
    } else {
        kingNewPoint = m_chessBoard->convertPoint(kingRow,kingCol+2);
        rookNewPoint = m_chessBoard->convertPoint(kingRow,kingCol+1);
    }
    clearSequenceMove();
    appendSequenceMove(rookPoint, rookNewPoint);
    appendSequenceMove(kingPoint, kingNewPoint);
    appendStandByMove();
    initSequenceMove(3);
}

void ApplicationController::clearSequenceMove() {
    m_robot->resetMoveSequene();
}
void ApplicationController::appendSequenceMove(Point start, Point stop, bool straightMove) {
    if(!straightMove) {
        float upAngles[6] = {45.0f,0.0f,45.0f,
                              45.0f,0.0f,45.0f};
        Point position[6] = {start,start,start,
                              stop,stop,stop};
        int captureStep[6] = {0,100,100,
                               100,0,0};
        int jointSteps[MAX_MOTOR];
        int numStep = 6;
        for(int seqStep = 0; seqStep < numStep; seqStep++)
        {
            calculateJoints(position[seqStep].x, position[seqStep].y, upAngles[seqStep], jointSteps);
            m_robot->appendMove(jointSteps,captureStep[seqStep]);
        }
    }
}
void ApplicationController::appendStandByMove() {
    int jointSteps[MAX_MOTOR];
    jointSteps[0] = 0;
    jointSteps[1] = 90;
    jointSteps[2] = 45;
    m_robot->appendMove(jointSteps,0);
}
void ApplicationController::initSequenceMove(int numberOfJoints) {
    m_robot->moveSequence(numberOfJoints);
}

