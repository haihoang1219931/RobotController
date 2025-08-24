#include "Robot.h"
#include "Motor.h"
#include "ApplicationController.h"
#include <math.h>
Robot::Robot(ApplicationController* app, int numMotor) :
    m_app(app),
    m_state(ROBOT_INIT),
    m_numMotor(numMotor),
    m_executeNumMotor(numMotor)
{
    for(int i=0;i< MAX_MOTOR; i++) {
        m_motorList[i] = new Motor(this,i);
    }
}

void Robot::setState(ROBOT_STATE newState) {
    if(m_state != newState) {
        m_state = newState;
        m_app->printf("ROBOT STATE: %d\r\n",m_state);
    }
}

void Robot::loop() {
    m_elapsedTime = m_app->getSystemTime() - m_startTime;
    m_app->printf("Robot time[%ld]\r\n", m_elapsedTime);
    switch(m_state) {
    case ROBOT_EXECUTE_GO_HOME: {
        executeGohome();
    }
        break;
    case ROBOT_EXECUTE_POSITION: {
        executeGohome();
    }
        break;
    case ROBOT_EXECUTE_DONE: {
        m_app->setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
    }
        break;
    }
}
void Robot::goHome() {
    m_app->printf("GO HOME\r\n");
    setState(ROBOT_EXECUTE_GO_HOME);
    //  m_app->msleep(100);
    for(int i=0; i< MAX_MOTOR; i++)
    {
        m_motorList[i]->initPlan(0,300,-1,true);
    }
    m_startTime = m_app->getSystemTime();
}

void Robot::executeGohome() {
    bool allMotorsAtHome = true;
    for(int i=0; i< m_executeNumMotor; i++)
    {
        m_app->printf("M[%d] Time[%d] P[%d] T[%d]\r\n",
                      i,m_elapsedTime,
                      m_motorList[i]->currentStep(),
                      m_motorList[i]->targetStep());
        if(!m_motorList[i]->isFinishExecution())
        {
            m_motorList[i]->executePlan();
            allMotorsAtHome = false;
        }
    }
    if(allMotorsAtHome) {
        setState(ROBOT_EXECUTE_DONE);
    }
}

int Robot::getNumMotor()
{
    return m_executeNumMotor;
}

void Robot::goToPosition(int* stepList, int numMotor)
{
    // calculate the step time for all motors in us
    setState(ROBOT_EXECUTE_POSITION);
    m_executeNumMotor = numMotor;
    int listSteps[MAX_MOTOR];
    int listTimeStep[MAX_MOTOR];
    int maxStep = 0;
    int minStep = 0;

    for(int i=0; i< numMotor; i++){
        listSteps[i] = abs(stepList[i] - m_motorList[i]->currentStep());
        if(listSteps[i] > maxStep) maxStep = listSteps[i];
        if(minStep == 0) minStep = listSteps[i];
        else if(listSteps[i] < minStep) minStep = listSteps[i];
    }
//    m_app->printf("minStep[%d] maxStep[%d]\r\n",
//           minStep,maxStep);
//    int minScale = (int)pow(10,(int)log10((double)maxStep));
    int minScale = 1;
//    m_app->printf("minScale[%d]\r\n",minScale);
    for(int i=0; i< numMotor; i++){
        listTimeStep[i] = round((float)(minScale * maxStep) / listSteps[i]);
        listTimeStep[i] = 1;
        m_app->printf("M[%d] [%d %d = %d] ",
               i,listSteps[i],listTimeStep[i],listSteps[i]*listTimeStep[i]);
    }
    m_app->printf("\r\n");
    for(int i=0; i< numMotor; i++){
        m_motorList[i]->initPlan(stepList[i],listTimeStep[i],0);
    }
    m_startTime = m_app->getSystemTime();
}

void Robot::executeGoToPosition() {
    bool allMotorsAtTarget = true;
    m_elapsedTime = m_app->getSystemTime() - m_startTime;
    for(int i=0; i< m_executeNumMotor; i++)
    {
        m_motorList[i]->executePlan();
    }
    if(allMotorsAtTarget) {
        setState(ROBOT_EXECUTE_DONE);
    }
}
//void Robot::goToPosition(int startCol, int startRow, int stopCol, int stopRow, bool attack, bool castle, char promote) {
//  m_app->printf("Go to Pos [%d,%d] to [%d,%d] attack[%s] castle[%s] promote[%c]\r\n",
//                  startCol, startRow, stopCol, stopRow, attack?"yes":"no", castle?"yes":"no", promote);
//  int centerCol = 10;
//  int centerRow = 0;
//  float squareLength = 20.0f;
//  float captureAngles[8] = {0.0f,0.0f,2000.0f,2000.0f,2000.0f,2000.0f,0.0f,0.0f};
//  float upAngles[8] = {100.0f,55.0f,55.0f,100.0f,100.0f,55.0f,55.0f,100.0f};
//  int positionRow[8] = {startRow,startRow,startRow,startRow,stopRow,stopRow,stopRow,stopRow};
//  int positionCol[8] = {startCol,startCol,startCol,startCol,stopCol,stopCol,stopCol,stopCol};
//  resetMoveSequene();
//  for(int seqStep = 0; seqStep < 8; seqStep++) {
//    float xPos = (float)(positionCol[seqStep]-centerCol) * squareLength;
//    float yPos = (float)(positionRow[seqStep]-centerRow) * squareLength;
//    float upAngle = (upAngles[seqStep] - 55.0f)/180.0f*PI;
//    float a1 = 47.6f + 228.0f;
//    float a2Sin = 55.2f;
//    float a2Cos = 50.1f + 84.3f*cos(upAngle)+21.8f+14.4f;
//    float a2 = (float)sqrt(a2Sin*a2Sin + a2Cos*a2Cos);
//    float q2Offset = atan(a2Sin/a2Cos);
//    float q1 = 0;
//    float q2 = 0;
//    m_app->printf("xPos[%d]\r\n",(int)xPos);
//    m_app->printf("yPos[%d]\r\n",(int)yPos);
//    m_app->printf("a1[%d]\r\n",(int)a1);
//    m_app->printf("a2[%d]\r\n",(int)a2);
//    calculateRobotArm(xPos, yPos, a1, a2, &q1, &q2);
//    m_app->printf("arm1Angle[%d] arm2Angle[%d]=[%d] arm3Angle[%d] q2Offset[%d]\r\n",
//    (int)(q1/PI*180.0f),
//    (int)((q2)/PI*180.0f),
//    (int)((PI-q2-q2Offset)/PI*180.0f),
//    (int)(upAngles[seqStep]),
//    (int)(q2Offset/PI*180.0f));
//    long armAngle[MOTOR::MOTOR_MAX] = {
//      (long)(q1/PI*180.0f*m_armRatio[MOTOR_BASE]),
//      (long)((PI-q2-q2Offset)/PI*180.0f*m_armRatio[MOTOR_ARM1]),
//      (long)(upAngles[seqStep]*m_armRatio[MOTOR_ARM2]),
//      (long)(captureAngles[seqStep])};
//    appendMoveSequence(armAngle,MOTOR::MOTOR_MAX);
//  }
//  setState(ROBOT_EXECUTE_SEQUENCE);
//  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
//}

long Robot::elapsedTime()
{
    return m_elapsedTime;
}

void Robot::moveStep(int motorID)
{
//    m_app
}

bool Robot::isLimitReached(int motorID,
                           MOTOR_LIMIT_TYPE limitType)
{
    return m_app->isLimitReached(motorID,limitType);
}

void Robot::getCurrentPosition(int* listCurrentStep, int* numMotor)
{
    *numMotor = m_executeNumMotor;
    for(int i=0; i< m_executeNumMotor; i++)
    {
        listCurrentStep[i] = m_motorList[i]->currentStep();
    }
}

Motor* Robot::getMotor(int motorID)
{
    return m_motorList[motorID];
}
