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
  }
}
void Robot::goHome() {
  setState(ROBOT_GO_HOME);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}
void Robot::executeGohome() {
  bool allMotorsAtHome = true;
  for(unsigned int button = 0; button < BUTTON_ID::BTN_MAX; button ++) {
    if(m_app->buttonState(button) == BUTTON_STATE::BUTTON_NOMAL) {
      allMotorsAtHome = false;
      break;
    }
  }
  if(allMotorsAtHome) {
    m_app->printf("ALL MOTOR ARE HOME\r\n");
    m_app->setMachineState(MACHINE_WAIT_COMMAND);
  }
}
    
void Robot::goToPosition(int startCol, int startRow, int stopCol, int stopRow, bool attack, bool castle, char promote) {
  m_moveSequence[0] = {false,{0,0,0},100};
  m_moveSequence[1] = {false,{0,900,900},100};
  m_moveSequence[2] = {false,{0,900,900},0};
  m_moveSequence[3] = {false,{0,700,700},0};
  m_moveSequence[4] = {false,{0,900,100},100};
  m_moveSequence[5] = {false,{0,1140,300},100};
  m_numberMove = 6;
  m_currentMoveID = 0;
  setState(ROBOT_EXECUTE_SEQUENCE);
  m_app->setMachineState(MACHINE_EXECUTE_COMMAND);
}
void Robot::executeGoToPosition() {
  if(!m_moveSequence[m_currentMoveID].moveInit) {
      for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
          m_app->setTargetPos(axis,m_moveSequence[m_currentMoveID].armAngle[axis]);
      }
      m_moveSequence[m_currentMoveID].moveInit = true;
  }
  bool allMotorsAtTarget = true;
  for(unsigned int axis=0; axis < (unsigned int)MOTOR::MOTOR_MAX; axis++) {
      if(m_app->distanceToGo(axis) != 0){
          allMotorsAtTarget = false;
          m_app->run(axis);
      }
  }
  
  if(allMotorsAtTarget) {
    m_app->msleep(100);
    m_currentMoveID++;
  }

  if(m_currentMoveID >= m_numberMove) {
    goHome();
  }
}