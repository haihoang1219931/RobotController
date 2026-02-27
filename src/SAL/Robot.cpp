#include "Robot.h"
#include "ApplicationController.h"
#include "SmoothMotion.h"
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
    for(uint32_t motorID=MOTOR_CAPTURE; motorID < MAX_MOTOR; motorID++) {
        m_motorList[motorID] = new SmoothMotion(motorID,this);
    }
}

void Robot::setMotorParam(int motorID, JointParam param)
{
    m_motorParamList[motorID] = param;
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
    m_app->enableHardwareTimer(false);
    m_requestMotorID = motorID;
    int startID = motorID == MAX_MOTOR ? 0 : motorID;
    int stopID = motorID == MAX_MOTOR ? MAX_MOTOR : motorID+1;
    m_app->printf("Request go home [%d-%d]\r\n",startID,stopID);
    for(int motor=startID; motor< stopID; motor++)
    {
        if(m_motorParamList[motor].active) {
            m_motorList[motor]->setupTarget(
                0,0xFFFF,0,
                m_motorParamList[motor].initAngle > m_motorParamList[motor].homeAngle ? -1 : 1,
                false,
                m_motorParamList[motor].homeStepTime, 0);
        }
    }
    m_startTime = m_app->getSystemTime();
    setState(ROBOT_EXECUTE_GO_HOME);
    m_app->enableHardwareTimer(true);
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
        if(!m_motorParamList[motor].active) continue;
        if(!m_app->isLimitReached(motor, MOTOR_LIMIT_HOME))
        {
#ifdef DEBUG_ROBOT
            m_app->printf("Robot M[%d] Time[%d] P[%d] T[%d]\r\n",
                        motor,m_elapsedTime,
                        m_motorParamList[motor].currentStep,
                        m_motorParamList[motor].targetStep);
            m_app->printf("Robot M[%d] gohome\r\n",motor);
#endif
            allMotorsAtHome = false;
        } else {
            m_motorParamList[motor].currentStep = angleToStep(motor, m_motorParamList[motor].homeAngle);
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
    m_motorParamList[motorID].targetStep = targetStep;
    m_motorParamList[motorID].homeStepTime = stepTime;
    m_motorParamList[motorID].direction = (targetStep > m_motorParamList[motorID].currentStep) ? 1 : -1;
    m_startTime = m_app->getSystemTime();
    setState(ROBOT_EXECUTE_POSITION);
}

long Robot::elapsedTime()
{
    return m_elapsedTime;
}

int Robot::angleToStep(int motorID, float angle)
{
    return (int)(angle * m_motorParamList[motorID].scale);
}

float Robot::stepToAngle(int motorID, int step)
{
    return (float)step / m_motorParamList[motorID].scale;
}
void Robot::currentStep(int* listCurrentStep, int* numMotor)
{
    *numMotor = MAX_MOTOR;
    for(int i=MOTOR_CAPTURE; i< MAX_MOTOR; i++)
    {
        listCurrentStep[i] = m_motorParamList[i].currentStep;
    }
}

void Robot::currentAngle(float* listCurrentAngle, int* numMotor)
{
    *numMotor = MAX_MOTOR;
    for(int i=MOTOR_CAPTURE; i< MAX_MOTOR; i++)
    {
        listCurrentAngle[i] = stepToAngle(i, m_motorParamList[i].currentStep);
    }
}

void Robot::armLength(float* listArmLength, int* numMotor)
{
    *numMotor = MAX_MOTOR;
    for(int i=MOTOR_CAPTURE; i< MAX_MOTOR; i++)
    {
        listArmLength[i] = m_motorParamList[i].length;
    }
}

int Robot::minStep(int motorID)
{
    return (int)(m_motorParamList[motorID].scale * m_motorParamList[motorID].minAngle);
}

int Robot::maxStep(int motorID)
{
    return (int)(m_motorParamList[motorID].scale * m_motorParamList[motorID].maxAngle);
}

float Robot::homeAngle(int motorID)
{
    return m_motorParamList[motorID].homeAngle;
}

int Robot::homeStep(int motorID)
{
    return angleToStep(motorID, m_motorParamList[motorID].homeAngle);
}

void Robot::executeSmoothMotion(int motorID)
{
    m_motorList[motorID]->motionControlLoop();
}

uint8_t Robot::statePulse(int motorID)
{
    return m_motorParamList[motorID].statePulse;
}

uint32_t Robot::numWaitPulse(int motorID)
{
    return m_motorParamList[motorID].numWaitPulse;
}

uint32_t Robot::countPulse(int motorID)
{
    return m_motorParamList[motorID].countPulse;
}

void Robot::updateStatePulse(int motorID, uint8_t newState)
{
    m_motorParamList[motorID].statePulse = newState;
}

void Robot::updateCountPulse(int motorID, uint32_t countPulse)
{
    m_motorParamList[motorID].countPulse = countPulse;
}

void Robot::updateNumWaitPulse(int motorID, uint32_t numWaitPulse)
{
    m_motorParamList[motorID].numWaitPulse = numWaitPulse;
}

void Robot::updateInitAngle(int motorID, float initAngle)
{
    m_motorParamList[motorID].currentStep = angleToStep(motorID,initAngle);
}

float Robot::armLength(int motorID)
{
    return m_motorParamList[motorID].length;
}

int Robot::currentStep(int motorID)
{
    return m_motorParamList[motorID].currentStep;
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
        if(!m_motorParamList[i].active) continue;
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

uint8_t Robot::pulseLoop(int motorID)
{
    uint8_t newStatePulse = m_app->executePulseLoop(motorID);
    m_motorParamList[motorID].statePulse = newStatePulse;
    return newStatePulse;
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
    m_app->enableMotionTask(false);              
    // Calculate time for each motor to reach target step, 
    // then start with the motor which has longest time to reach target step
    float maxTime = 0;
    for(int i=MOTOR_ARM1; i< MAX_MOTOR; i++) {
        if(!m_motorParamList[i].active) continue;
        m_motorParamList[i].targetStep = m_moveSequence[m_curMove].jointSteps[i].steps;
        m_motorParamList[i].startStep = m_motorParamList[i].currentStep;
        m_motorParamList[i].direction = (m_motorParamList[i].targetStep > m_motorParamList[i].currentStep) ? 1 : -1;   
        float numStep = (float)abs(m_motorParamList[i].targetStep - m_motorParamList[i].currentStep);
        float time = (float)numStep / m_motorParamList[i].maxSpeed;
        if(time > maxTime) maxTime = time;
        m_app->printf("Motor[%d] numStep[%d] time[%d] => Max[%f]\r\n", i, numStep, time, maxTime);
    }

    // Set step time for each motor
    for(int i=MOTOR_ARM1; i< MAX_MOTOR; i++) {
        if(!m_motorParamList[i].active) continue;
        float numStep = (float)abs(m_motorParamList[i].targetStep - m_motorParamList[i].currentStep);
        int delayTime = (int)(maxTime / numStep * 40000.0f);
        int delayTimeStart = (int)(delayTime * numStep * 0.1);
        delayTimeStart = delayTime;
        m_motorList[i]->setupTarget(
            (int)(numStep*0.0f), 
            (int)(numStep*1.0f), 
            (int)(numStep*0.0f), 
            m_motorParamList[i].direction, false, delayTime, 21);
        m_app->printf("Motor[%d] numStep[%d] delayTime[%d]\r\n", i, (int)numStep, delayTime);
    }

    // Initiate direction for each motor
    for(int i=0; i< MAX_MOTOR; i++) {
        if(!m_motorParamList[i].active) continue;
        m_app->initDirection(i, m_motorParamList[i].direction);
    }
    m_app->enableMotionTask(true);

    m_sequenceState = ROBOT_MOVE_EXECUTE;
}

void Robot::gotoTarget()
{
//    bool allMotorsFinished = true;
//    for(int motor = 0; motor< MAX_MOTOR; motor++)
//    {
//        if(!m_motorParamList[motor].active) continue;
//        m_motorParamList[motor].currentStep = m_app->readNumStepsFeedback(motor);
//        if(m_motorParamList[motor].currentStep != m_motorParamList[motor].targetStep) {
//            allMotorsFinished = false;
//        }
//    }
//    if(allMotorsFinished) {
//        m_app->printf("======All motor finished\r\n");
//        m_sequenceState = ROBOT_MOVE_DONE;
//    }
}

void Robot::capture()
{
//    bool captureDone = true;
//    if(captureDone) {
//        if(m_curMove < m_numMove-1) {
//            m_curMove++;
//            m_sequenceState = ROBOT_MOVE_INIT;
//        } else {
//            m_app->enableMotionTask(false);
//            m_sequenceState = ROBOT_MOVE_DONE;
//        }
//    }
}
