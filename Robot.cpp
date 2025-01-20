#include "Robot.h"
#include "Button.h"
#include "ApplicationController.h"
Robot::Robot(ApplicationController* app) :
    m_app(app),
    m_state(ROBOT_INIT)
{

}

void Robot::setState(ROBOT_STATE newState) {
  if(m_state != newState) {
    m_state = newState;
    m_app->printf("ROBOT STATE: %d\r\n",m_state);
  }
}

void Robot::loop() {
  switch(m_state) {
    case ROBOT_GO_HOME: {
      executeGohome();
    }
    break;
    case ROBOT_EXECUTE_SEQUENCE: {
      executeGoToPosition();
    }
    break;
    case ROBOT_EXECUTE_ROTATE_COMMAND: {
      executeRotateAngle();
    }
    break;
  }
}
void Robot::goHome() {
  for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
    m_app->setMaxSpeed (axis, 4000);
    m_app->setSpeed(axis,0);
    m_app->setAcceleration(axis,500);
    
  }
  m_app->setSpeed(0, -100);
  m_app->setSpeed(1, -100);
  m_app->setSpeed(2, -100);

  setState(ROBOT_GO_HOME);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}

void Robot::rotateAngle(MOTOR motorID, long angle, long speed) {
  m_motorID = motorID;
  m_app->printf("MOVE MOTOR[%d] to POS[%ld] SPEED[%ld]\r\n", motorID, angle, speed);
  m_app->setSpeed(motorID, speed);
  m_app->setTargetPos(motorID,angle);
  setState(ROBOT_EXECUTE_ROTATE_COMMAND);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}

void Robot::executeGohome() {
  bool allMotorsAtHome = true;
  for(unsigned int button = 0; button < BUTTON_ID::BTN_MAX; button ++) {
    if(m_app->buttonState(button) == BUTTON_STATE::BUTTON_NOMAL) {
      allMotorsAtHome = false;
      m_app->runSpeed(button);
    }else {
      m_app->setCurrentPosition(button, 0);
    }
  }
  if(allMotorsAtHome) {
    m_app->printf("ALL MOTOR ARE HOME\r\n");
    m_app->setMachineState(MACHINE_WAIT_COMMAND);
  }
}
    
void Robot::goToPosition(int startCol, int startRow, int stopCol, int stopRow, bool attack, bool castle, char promote) {
  m_moveSequence[0] = {false,{0,0,0},100};
  m_moveSequence[1] = {false,{0,300,600},100};
  m_moveSequence[2] = {false,{0,300,600},0};
  m_moveSequence[3] = {false,{0,300,450},0};
  m_moveSequence[4] = {false,{0,900,300},0};
  m_moveSequence[5] = {false,{0,900,300},100};
  m_moveSequence[6] = {false,{0,900,100},100};
  m_moveSequence[7] = {false,{0,0,0},100};
  m_numberMove = 8;
  m_currentMoveID = 0;

  for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
    m_app->setMaxSpeed(axis, 2000.0);
    m_app->setAcceleration(axis, 500.0);
  }

  setState(ROBOT_EXECUTE_SEQUENCE);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}
void Robot::executeGoToPosition() {
  if(!m_moveSequence[m_currentMoveID].moveInit) {
      for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
          m_app->setTargetPos(axis,m_moveSequence[m_currentMoveID].armAngle[axis]);
      }
      // control servo
      m_servoAngle = m_moveSequence[m_currentMoveID].crawlAngle;
      m_app->setServoAngle(m_moveSequence[m_currentMoveID].crawlAngle);
      m_servoStartTime = m_app->getSystemTimeInMillis();
      m_moveSequence[m_currentMoveID].moveInit = true;
  }
  bool allMotorsAtTarget = true;
  for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
      if(m_app->distanceToGo(axis) != 0){
          allMotorsAtTarget = false;
          m_app->run(axis);
      }
  }
  long servoRunTime = m_app->getSystemTimeInMillis() - m_servoStartTime;
  if(allMotorsAtTarget && servoRunTime >= 1000) {
    m_app->msleep(100);
    m_currentMoveID++;
  }

  if(m_currentMoveID >= m_numberMove) {
    goHome();
  }
}

void Robot::executeRotateAngle() {
  if(m_app->buttonState((BUTTON_ID)m_motorID) != BUTTON_STATE::BUTTON_NOMAL && m_app->speed(m_motorID) < 0) {
    m_app->printf("MOTOR[%d] at limit inverse speed\r\n", m_motorID);
    m_app->setMachineState(MACHINE_WAIT_COMMAND);
  } else {
    if(m_app->distanceToGo(m_motorID) != 0) {
      m_app->run(m_motorID);
    } else {
      m_app->printf("MOTOR[%d] reached[%ld]\r\n", m_motorID, m_app->currentPosition(m_motorID));
      m_app->setMachineState(MACHINE_WAIT_COMMAND);
    }
  }
}