#include "ApplicationController.h"
#include "Button.h"
#include "Robot.h"
#include "CommandReader.h"
#include "Arduino.h"
ApplicationController::ApplicationController()
{
    for(int i=0;i< BUTTON_ID::BTN_MAX; i++) {
        m_listButton[i] = new Button(this,i);
    }
    m_commandReader = new CommandReader(this);
    m_robot = new Robot(this);
}

ApplicationController::~ApplicationController() {

}

void ApplicationController::loop() {
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
  }
}

void ApplicationController::checkAllButtonState() {
    for(int i=0;i< BUTTON_ID::BTN_MAX; i++) {
        m_listButton[i]->checkState();
    }
}

int ApplicationController::buttonState(BUTTON_ID buttonID) {
    return m_listButton[buttonID]->buttonState();
}

MACHINE_STATE ApplicationController::stateMachine() {
    return m_machineState;
}

void ApplicationController::setMachineState(MACHINE_STATE machineState) {
    if(machineState != m_machineState) {
        m_machineState = machineState;
        this->printf("APP STATE: %d\r\n",m_machineState);
    }
}

void ApplicationController::executeCommand(char* command) {
  if(command[0] == 'H') {
    m_robot->goHome();
  } else if(command[0] == 'P'){
    m_robot->goHome();
  }
}