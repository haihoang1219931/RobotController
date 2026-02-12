#include "Robot.h"
#include "Motor.h"
#include "ApplicationController.h"
#include <math.h>
#include <string.h>

#ifdef abs
#undef abs
#endif

#define abs(x) ((x)>0?(x):-(x))

Robot::Robot(ApplicationController* app) :
    m_app(app),
    m_state(ROBOT_INIT)
{
    for(int i=MOTOR_CAPTURE;i< MAX_MOTOR; i++) {
        m_motorList[i] = new Motor(this,i);
    }
}

void Robot::setMotorParam(int motorID, JointParam param)
{
    m_motorList[motorID]->setParam(param);
}

void Robot::setState(ROBOT_STATE newState) {
    if(m_state != newState) {
        m_state = newState;
        m_app->printf("ROBOT STATE: %d\r\n",m_state);
    }
}

void Robot::loop() {
    m_elapsedTime = m_app->getSystemTime() - m_startTime;
#ifdef DEBUG_ROBOT
    m_app->printf("Robot time[%ld]\r\n", m_elapsedTime);
#endif
    switch(m_state) {
    case ROBOT_EXECUTE_GO_HOME: {
        executeGohome();
    }
        break;
    case ROBOT_EXECUTE_POSITION: {
        executeGoPosition();
    }
        break;
    case ROBOT_EXECUTE_SEQUENCE: {
        executeMoveSequence();
    }
        break;
    case ROBOT_EXECUTE_DONE: {
        m_app->setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
    }
        break;
    }
}
void Robot::requestGoHome(int motorID) {
    m_app->printf("GO HOME\r\n");
    m_requestMotorID = motorID;
    int startID = motorID == MAX_MOTOR ? 0 : motorID;
    int stopID = motorID == MAX_MOTOR ? MAX_MOTOR : motorID+1;
    for(int i=startID; i< stopID; i++)
    {
        if(m_motorList[i]->isActive())
            m_motorList[i]->initGoHome();
    }     
    m_startTime = m_app->getSystemTime();
    setState(ROBOT_EXECUTE_GO_HOME);
}

void Robot::executeGohome() {
    bool allMotorsAtHome = true;
#ifdef DEBUG_ROBOT
    m_app->printf("Robot executeGohome M[%d]\r\n",m_requestMotorID);
#endif
    int startID = m_requestMotorID == MAX_MOTOR ?
                                            MOTOR_CAPTURE : m_requestMotorID;
    int stopID = m_requestMotorID == MAX_MOTOR ?
                                            MAX_MOTOR : m_requestMotorID+1;
    for(int motor = startID; motor< stopID; motor++)
    {
        if(!m_motorList[motor]->isActive()) continue;
        if(!m_motorList[motor]->isFinishExecution())
        {
#ifdef DEBUG_ROBOT
            m_app->printf("Robot M[%d] Time[%d] P[%d] T[%d]\r\n",
                        motor,m_elapsedTime,
                        m_motorList[motor]->currentStep(),
                        m_motorList[motor]->targetStep());
            m_app->printf("Robot M[%d] gohome\r\n",motor);
#endif
            m_motorList[motor]->executePlan();
            allMotorsAtHome = false;
        }
    }
    if(allMotorsAtHome) {
        m_app->harwareStop(m_requestMotorID);
        setState(ROBOT_EXECUTE_DONE);
    }
}

void Robot::requestGoPosition(int motorID, int targetStep, int stepTime, bool isRelativeMove)
{
    m_requestMotorID = motorID;
    m_motorList[motorID]->initPlan(targetStep, stepTime, isRelativeMove);
    m_startTime = m_app->getSystemTime();
    setState(ROBOT_EXECUTE_POSITION);
}

void Robot::executeGoPosition()
{
    bool allMotorsDone = true;
#ifdef DEBUG_ROBOT
    m_app->printf("Robot executeGoPosition M[%d]\r\n",m_requestMotorID);
#endif
    int startID = m_requestMotorID == MAX_MOTOR ?
                                            MOTOR_CAPTURE : m_requestMotorID;
    int stopID = m_requestMotorID == MAX_MOTOR ?
                                            MAX_MOTOR : m_requestMotorID+1;
    for(int motor = startID; motor< stopID; motor++)
    {
        if(!m_motorList[motor]->isActive()) continue;
        if(!m_motorList[motor]->isFinishExecution())
        {
#ifdef DEBUG_ROBOT
            m_app->printf("Robot M[%d] Time[%d] P[%d] T[%d]\r\n",
                        motor,m_elapsedTime,
                        m_motorList[motor]->currentStep(),
                        m_motorList[motor]->targetStep());
#endif
            m_motorList[motor]->executePlan();
            allMotorsDone = false;
        }
    }
    if(allMotorsDone) {
        m_app->harwareStop(m_requestMotorID);
        setState(ROBOT_EXECUTE_DONE);
    }
}

long Robot::elapsedTime()
{
    return m_elapsedTime;
}

void Robot::initDirection(int motorID, int direction)
{
    m_app->initDirection(motorID, direction);
}

void Robot::moveStep(int motorID, int currentStep, int nextStep)
{
#ifdef DEBUG_ROBOT
    m_app->printf("Robot M[%d] moveStep\r\n",motorID);
#endif
    m_app->moveStep(motorID, currentStep, nextStep);
}

void Robot::moveDoneAction(int motorID)
{
    m_app->moveDoneAction(motorID);
}

bool Robot::isLimitReached(int motorID,
                           MOTOR_LIMIT_TYPE limitType)
{
    return m_app->isLimitReached(motorID,limitType);
}

int Robot::homeStep(int motorID)
{
    return m_motorList[motorID]->homeStep();
}

int Robot::motorCurrentStep(int motorID)
{
    return m_motorList[motorID]->currentStep();
}

void Robot::setCurrentStep(int motorID, int step)
{
    m_motorList[motorID]->setCurrentStep(step);
}

void Robot::setDir(int motorID, int dir)
{
    m_motorList[motorID]->setDir(dir);
}

int Robot::dir(int motorID)
{
    return m_motorList[motorID]->dir();
}

int Robot::angleToStep(int motorID, float angle)
{
    return m_motorList[motorID]->angleToStep(angle);
}

float Robot::stepToAngle(int motorID, int step)
{
    return m_motorList[motorID]->stepToAngle(step);
}
void Robot::currentStep(int* listCurrentStep, int* numMotor)
{
    *numMotor = MAX_MOTOR;
    for(int i=MOTOR_CAPTURE; i< MAX_MOTOR; i++)
    {
        listCurrentStep[i] = m_motorList[i]->currentStep();
    }
}

void Robot::currentAngle(float* listCurrentAngle, int* numMotor)
{
    *numMotor = MAX_MOTOR;
    for(int i=MOTOR_CAPTURE; i< MAX_MOTOR; i++)
    {
        listCurrentAngle[i] = m_motorList[i]->currentAngle();
    }
}

void Robot::armLength(float* listArmLength, int* numMotor)
{
    *numMotor = MAX_MOTOR;
    for(int i=MOTOR_CAPTURE; i< MAX_MOTOR; i++)
    {
        listArmLength[i] = m_motorList[i]->length();
    }
}

int Robot::minStep(int motorID)
{
    return m_motorList[motorID]->minStep();
}

int Robot::maxStep(int motorID)
{
    return m_motorList[motorID]->maxStep();
}

float Robot::homeAngle(int motorID)
{
    return m_motorList[motorID]->homeAngle();
}

float Robot::armLength(int motorID)
{
    return m_motorList[motorID]->length();
}

int Robot::currentStep(int motorID)
{
    return m_motorList[motorID]->currentStep();
}

void Robot::resetMoveSequene()
{
    m_curMove = 0;
    m_numMove = 0;
}

void Robot::appendMove(int* jointSteps)
{
//    memcpy(m_moveSequence[m_numMove].jointSteps,jointSteps,sizeof(int)*MAX_MOTOR);
    for(int i=0; i< MAX_MOTOR; i++) {
        if(!m_motorList[i]->isActive()) continue;
        m_moveSequence[m_numMove].jointSteps[i].steps = jointSteps[i];
        m_app->printf("Robot::appendMove[%d] M[%d] step[%d]\r\n",
                      m_numMove,
                      i,m_moveSequence[m_numMove].jointSteps[i].steps);
    }
    m_numMove++;
}

void Robot::moveSequence(int motorID)
{
#ifdef DEBUG_ROBOT
    m_app->printf("move sequence\r\n");
#endif
    m_requestMotorID = motorID;
    setState(ROBOT_EXECUTE_SEQUENCE);
    m_sequenceState = ROBOT_MOVE_INIT;
}

void Robot::executeMoveSequence()
{
    switch (m_sequenceState) {
    case ROBOT_MOVE_INIT:{
        initMove();
    }
        break;
    case ROBOT_MOVE_EXECUTE: {
        gotoTarget();
    }
        break;
    case ROBOT_MOVE_CAPTURE: {
        capture();
    }
        break;
    case ROBOT_MOVE_DONE: {
        setState(ROBOT_EXECUTE_DONE);
    }
        break;
    }

}

void Robot::initMove()
{
    m_app->printf("============= Init Move [%02d/%02d] =============\r\n",
                  m_curMove, m_numMove);
    int listSteps[MAX_MOTOR];
    int listTimeStep[MAX_MOTOR];
    int maxStep = 0;
    int minStep = 0;

    for(int i=MOTOR_ARM1; i< MAX_MOTOR; i++){
        if(!m_motorList[i]->isActive()) continue;
        m_app->printf("Robot InitMove M[%d] Steps[%d]\r\n",
               i,m_moveSequence[m_curMove].jointSteps[i].steps);
        listSteps[i] = abs(m_moveSequence[m_curMove].jointSteps[i].steps - m_motorList[i]->currentStep());
        if(listSteps[i] > maxStep) maxStep = listSteps[i];
        if(listSteps[i] == 0) continue;
        if(minStep == 0) minStep = listSteps[i];
        else if(listSteps[i] < minStep) minStep = listSteps[i];
    }
    m_app->printf("minStep[%d] maxStep[%d]\r\n",
           minStep,maxStep);
//    int minScale = (int)pow(10,(int)log10((double)maxStep));
    int minScale = 1;
//    m_app->printf("minScale[%d]\r\n",minScale);
    for(int i=MOTOR_ARM1; i< MAX_MOTOR; i++){
        if(!m_motorList[i]->isActive()) continue;
        if(listSteps[i] != 0)
            listTimeStep[i] = round((float)(minScale * maxStep) / listSteps[i]);
        else listTimeStep[i] = 0;
        // m_app->printf("Robot     M[%d] [%d %d = %d]\r\n",
        //        i,listSteps[i],listTimeStep[i],listSteps[i]*listTimeStep[i]);
    }
    // m_app->printf("\r\n");
    for(int i=MOTOR_ARM1; i< MAX_MOTOR; i++){
        if(!m_motorList[i]->isActive()) continue;
        m_motorList[i]->initPlan(m_moveSequence[m_curMove].jointSteps[i].steps,
                                 listTimeStep[i],false);
    }
    // Capture
    int stepTimeCapture =
            m_moveSequence[m_curMove].jointSteps[MOTOR_CAPTURE].steps != m_motorList[MOTOR_CAPTURE]->currentStep() ? 1 : 0;
    m_motorList[MOTOR_CAPTURE]->initPlan(
                m_moveSequence[m_curMove].jointSteps[MOTOR_CAPTURE].steps,
                stepTimeCapture,false);
    // start time
    m_startTime = m_app->getSystemTime();
    m_sequenceState = ROBOT_MOVE_EXECUTE;
}

void Robot::gotoTarget()
{
//    m_sequenceState = ROBOT_MOVE_CAPTURE;
//    return;
    int startID = m_requestMotorID == MAX_MOTOR ?
                                            MOTOR_ARM1 : m_requestMotorID;
    int stopID = m_requestMotorID == MAX_MOTOR ?
                                            MAX_MOTOR : m_requestMotorID+1;
    bool allMotorsFinished = true;
    for(int i=startID; i< stopID; i++)
    {
        if(!m_motorList[i]->isActive()) continue;
        if(!m_motorList[i]->isFinishExecution())
        {
            m_motorList[i]->executePlan();
            allMotorsFinished = false;
        }
    }
    if(allMotorsFinished) {
        m_app->printf("======All motor finished\r\n");
        m_sequenceState = ROBOT_MOVE_CAPTURE;
    }
}

void Robot::capture()
{
    bool captureDone = true;
#ifdef DEBUG_ROBOT
    m_app->printf("Cap Time[%d] P[%d] T[%d]\r\n",
                  m_elapsedTime,
                  m_motorList[MOTOR_CAPTURE]->currentStep(),
                  m_motorList[MOTOR_CAPTURE]->targetStep());
#endif
    if(!m_motorList[MOTOR_CAPTURE]->isFinishExecution())
    {
        m_motorList[MOTOR_CAPTURE]->executePlan();
        captureDone = false;
    }
    if(captureDone) {
        if(m_curMove < m_numMove-1) {
            m_curMove++;
            m_sequenceState = ROBOT_MOVE_INIT;
        } else {
            m_sequenceState = ROBOT_MOVE_DONE;
        }
    }
}
