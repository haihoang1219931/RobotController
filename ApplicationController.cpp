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
      case MACHINE_EXECUTE_COMMAND_DONE: {
        this->printf("_%04d DON",m_comCommandID);
        setMachineState(MACHINE_WAIT_COMMAND);
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
  } else if(command[0] == 'e') {
    this->enableEngine(true);
  } else if(command[0] == 'd') {
    this->enableEngine(false);
  } else if(command[0] == 'h') {
    m_robot->goHome();
  } else if(command[0] == 'a') {
    if(strlen(command)<21) {
      this->printf("All Params: a [base] [arm1] [arm2] [servo]\r\n");
      return;
    }
    char angleStr[16];
    angleStr[0] = command[2];
    angleStr[1] = command[3];
    angleStr[2] = command[4];
    angleStr[3] = command[5];
    angleStr[4] = 0;
    long angleBase = atoi(angleStr);

    angleStr[0] = command[7];
    angleStr[1] = command[8];
    angleStr[2] = command[9];
    angleStr[3] = command[10];
    angleStr[4] = 0;
    long angleArm1 = atoi(angleStr);

    angleStr[0] = command[12];
    angleStr[1] = command[13];
    angleStr[2] = command[14];
    angleStr[3] = command[15];
    angleStr[4] = 0;
    long angleArm2 = atoi(angleStr);

    angleStr[0] = command[17];
    angleStr[1] = command[18];
    angleStr[2] = command[19];
    angleStr[3] = command[20];
    angleStr[4] = 0;
    long angleStepper = atoi(angleStr);
    m_robot->ablsoluteAngle(angleBase,angleArm1,angleArm2,angleStepper);
  } else if(command[0] == 'p'){
    if(strlen(command)<9) {
      this->printf("Mission Params: p [startCol][startRow][stopCol][stopRow][a/n][c/n][promote piece]\r\n");
      return;
    }
    int startCol = command[2]-'0';
    int startRow = command[3]-'0';
    int stopCol = command[4]-'0';
    int stopRow = command[5]-'0';
    bool attack = command[6] == 'a';
    bool castle = command[7] == 'c';
    char promote = command[8];
    m_robot->goToPosition(startCol, startRow, stopCol, stopRow, attack, castle, promote);
  } else  if(command[0] == 'r') {
    if(strlen(command)<12) {
      this->printf("Mission Params: r[motor:0-3] [position:0000-9999] [speed:0000-9999]\r\n");
      return;
    }
    int motorID = command[1]-'0';
    char angleStr[16];
    angleStr[0] = command[3];
    angleStr[1] = command[4];
    angleStr[2] = command[5];
    angleStr[3] = command[6];
    angleStr[4] = 0;
    long angle = atoi(angleStr);

    char speedStr[16];
    speedStr[0] = command[8];
    speedStr[1] = command[9];
    speedStr[2] = command[10];
    speedStr[3] = command[11];
    speedStr[4] = 0;
    float speed = atof(speedStr);
    m_robot->rotateAngle(motorID, angle, speed);
  }
}