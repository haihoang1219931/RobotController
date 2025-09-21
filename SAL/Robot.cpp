#include "Robot.h"
#include "Motor.h"
#include "ApplicationController.h"
#include <math.h>
#include <string.h>
Robot::Robot(ApplicationController* app, int numMotor) :
    m_app(app),
    m_state(ROBOT_INIT),
    m_numMotor(numMotor),
    m_executeNumMotor(numMotor)
{
    for(int i=0;i< MAX_MOTOR; i++) {
        m_motorList[i] = new Motor(this,i);
    }
    m_capture = new Motor(this,-1);
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

void Robot::goToPosition(int* stepList, int numMotor, MOTION_SPACES motionSpace)
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
        m_app->printf("    M[%d] [%d %d = %d]\r\n",
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

void Robot::getCurrentPosition(int* listCurrentStep, int* numMotor, float* captureStep)
{
    *numMotor = MAX_MOTOR;
    for(int i=0; i< MAX_MOTOR; i++)
    {
        listCurrentStep[i] = m_motorList[i]->currentStep();
    }
    *captureStep = m_capture->currentStep();
}

Motor* Robot::getMotor(int motorID)
{
    return m_motorList[motorID];
}

void Robot::resetMoveSequene()
{
    m_curMove = 0;
    m_numMove = 0;
}

void Robot::appendMove(int* jointSteps, int captureStep)
{
    memcpy(m_moveSequence[m_numMove].jointSteps,jointSteps,sizeof(int)*MAX_MOTOR);
    m_moveSequence[m_numMove].captureStep = captureStep;
    m_numMove++;
}

void Robot::moveSequence(int numMotor)
{
    m_app->printf("move sequence\r\n");
    m_executeNumMotor = numMotor;
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

    for(int i=0; i< m_executeNumMotor; i++){
        listSteps[i] = abs(m_moveSequence[m_curMove].jointSteps[i] - m_motorList[i]->currentStep());
        if(listSteps[i] > maxStep) maxStep = listSteps[i];
        if(minStep == 0) minStep = listSteps[i];
        else if(listSteps[i] < minStep) minStep = listSteps[i];
    }
//    m_app->printf("minStep[%d] maxStep[%d]\r\n",
//           minStep,maxStep);
//    int minScale = (int)pow(10,(int)log10((double)maxStep));
    int minScale = 1;
//    m_app->printf("minScale[%d]\r\n",minScale);
    for(int i=0; i< m_executeNumMotor; i++){
        listTimeStep[i] = round((float)(minScale * maxStep) / listSteps[i]);
        m_app->printf("     M[%d] [%d %d = %d]\r\n",
               i,listSteps[i],listTimeStep[i],listSteps[i]*listTimeStep[i]);
    }
    m_app->printf("\r\n");
    for(int i=0; i< m_executeNumMotor; i++){
        m_motorList[i]->initPlan(m_moveSequence[m_curMove].jointSteps[i],listTimeStep[i],0);
    }

    // init capture
    m_capture->initPlan(m_moveSequence[m_curMove].captureStep,1,0);

    // start time
    m_startTime = m_app->getSystemTime();

    m_sequenceState = ROBOT_MOVE_EXECUTE;
}

void Robot::gotoTarget()
{
//    m_sequenceState = ROBOT_MOVE_CAPTURE;
//    return;
    bool allMotorsAtHome = true;
    for(int i=0; i< m_executeNumMotor; i++)
    {
//        m_app->printf("M[%d] Time[%d] P[%d] T[%d]\r\n",
//                      i,m_elapsedTime,
//                      m_motorList[i]->currentStep(),
//                      m_motorList[i]->targetStep());
        if(!m_motorList[i]->isFinishExecution())
        {
            m_motorList[i]->executePlan();
            allMotorsAtHome = false;
        }
    }
    if(allMotorsAtHome) {
        m_sequenceState = ROBOT_MOVE_CAPTURE;
    }
}

void Robot::capture()
{
    bool captureDone = true;
    m_app->printf("Cap Time[%d] P[%d] T[%d]\r\n",
                  m_elapsedTime,
                  m_capture->currentStep(),
                  m_capture->targetStep());
    if(!m_capture->isFinishExecution())
    {
        m_capture->executePlan();
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
