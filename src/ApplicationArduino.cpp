#include "ApplicationArduino.h"
#include "MiniStepper_driver.h"
#include "Stepper_driver.h"
#include "Servo_driver.h"
#include "SAL/Button.h"
#include "SAL/ChessBoard.h"
#include "SAL/Robot.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "SAL/StdTypes.h"

#define NONE_PIN  0xFF
#define MINISTEPPER_UPDOWN_ID 0
#define MINISTEPPER_GRIPPER_ID 1
const int enPin=8;
const int stepXPin = 2; //X.STEP
const int dirXPin = 5; // X.DIR
const int stepYPin = 3; //Y.STEP
const int dirYPin = 6; // Y.DIR

const int limitX = 9; // X.LIMIT
const int limitY = 10; // Y.LIMIT

const int miniStepperUpdownPin1 = 34;
const int miniStepperUpdownPin2 = 36;
const int miniStepperUpdownPin3 = 38;
const int miniStepperUpdownPin4 = 40;

const int miniStepperGripperPin1 = 35;
const int miniStepperGripperPin2 = 37;
const int miniStepperGripperPin3 = 39;
const int miniStepperGripperPin4 = 41;

const int limitUpdown = 11;
const int limitGripper = A8;

volatile char m_buffer[128];
ApplicationArduino::ApplicationArduino()
{
    memset(m_buttonPin,NONE_PIN,sizeof(m_buttonPin));
    m_buttonPin[MOTOR_ARM1] = limitX;
    m_buttonPin[MOTOR_ARM2] = limitY;
    m_buttonPin[MOTOR_ARM5] = limitUpdown;

    pinMode(m_buttonPin[MOTOR_ARM1], INPUT_PULLUP);
    pinMode(m_buttonPin[MOTOR_ARM2], INPUT_PULLUP);
    pinMode(m_buttonPin[MOTOR_ARM5], INPUT_PULLUP);
    
    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, HIGH);

    m_listMiniStepper[MINISTEPPER_UPDOWN_ID] = new MiniStepper_driver(
      miniStepperUpdownPin1,miniStepperUpdownPin2,miniStepperUpdownPin3,miniStepperUpdownPin4);
    m_listMiniStepper[MINISTEPPER_GRIPPER_ID] = new MiniStepper_driver(
      miniStepperGripperPin1,miniStepperGripperPin2,miniStepperGripperPin3,miniStepperGripperPin4);

    m_listStepper[MOTOR_ARM1] = new Stepper_driver(enPin,stepXPin, dirXPin);
    m_listStepper[MOTOR_ARM2] = new Stepper_driver(enPin,stepYPin, dirYPin);
    initRobot();
}

ApplicationArduino::~ApplicationArduino()
{

}

void ApplicationArduino::initRobot()
{
    m_chessBoard->setChessBoardPosX(13-13*8/2);
    m_chessBoard->setChessBoardPosY(46);
    m_chessBoard->setChessBoardSize(13*8);
    m_chessBoard->setDropZoneSpace(13);

    JointParam armPrams[MAX_MOTOR] = {
    // active |   scale   |length|init angle|home angle|home step|min angle|max angle|
        {true,  1.0f*1.0f,     0,    100,        0,       1000,      0,       250   },
        {true,  18.0f*1.0f,  275,      0,       -17,       100,      0,       150   },
        {true,  4.0f*1.0f,    55,    140,       50,        100,      0,       210   },
        {false, 1.0f*1.0f,    25,    130,      135,          1,      0,         0   },
        {false, 1.0f*1.0f, 32+35,    180,      135,          1,      0,         0   },
        {true,  2.0f*1.0f,    85,     20,       45,        100,      0,        45   }
    };

    for(int motor= MOTOR_CAPTURE; motor<= MOTOR_ARM5; motor++) {
        m_robot->setMotorParam(motor,armPrams[motor]);
    }
}

int ApplicationArduino::printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    int rc = vsprintf(m_buffer, fmt, args);
    va_end(args);
    Serial.print((const char*)m_buffer);
    return rc;
}
void ApplicationArduino::msleep(int millis) {
  delay(millis);
}

long ApplicationArduino::getSystemTime() {
	return m_appTimer;
}

void ApplicationArduino::specificPlatformGohome(int motorID)
{
  // Not used
}

void ApplicationArduino::harwareStop(int motorID = MAX_MOTOR)
{
  // Not used
}

void ApplicationArduino::checkInput(){
	for(unsigned int btnID = 0; btnID< MAX_BUTTON; btnID++) {
    if(m_buttonPin[btnID] != NONE_PIN) {
      pinMode(m_buttonPin[btnID], INPUT_PULLUP);
      storeButtonState(btnID, digitalRead(m_buttonPin[btnID]) == LOW);
      // this->printf("button[%d] %s\r\n",btnID, digitalRead(m_buttonPin[btnID]) == LOW ? "pressed" : "normal");
    }
	}
  m_limitGripperValue = analogRead(limitGripper);
}

int ApplicationArduino::readSerial(char* output, int length) {
  String command;
  while(Serial.available()) {
    delay(3);
    char c = Serial.read();
    command += c;  
  }
  if(command.length()>0) {
    Serial.print(command);
    for(int i=0; i< command.length(); i++) {
      output[i] = command.charAt(i);
#ifdef DEBUG_SERIAL
      Serial.print(output[i],HEX);
      Serial.print(" ");
#endif
    }
#ifdef DEBUG_SERIAL
    Serial.print("\n");
#endif
  }
  return command.length();
}

bool ApplicationArduino::isLimitReached(int motorID, MOTOR_LIMIT_TYPE limitType)
                      {
  bool limitReached = true;
  switch(motorID){
    case MOTOR::MOTOR_ARM1: {
      limitReached = limitType == MOTOR_LIMIT_MIN || limitType == MOTOR_LIMIT_HOME ? 
                    (m_buttonList[MOTOR_ARM1]->buttonState() != BUTTON_STATE::BUTTON_NOMAL) :
                    false;
    }
    break;
    case MOTOR::MOTOR_ARM2: {
      limitReached = limitType == MOTOR_LIMIT_MIN || limitType == MOTOR_LIMIT_HOME ? 
                    (m_buttonList[MOTOR_ARM2]->buttonState() != BUTTON_STATE::BUTTON_NOMAL) :
                    false;
    }
    break;
    case MOTOR::MOTOR_ARM5: {
      limitReached = limitType == MOTOR_LIMIT_MIN || limitType == MOTOR_LIMIT_HOME ? 
                    (m_buttonList[MOTOR_ARM5]->buttonState() != BUTTON_STATE::BUTTON_NOMAL) :
                    false;
    }
    break;
    case MOTOR::MOTOR_CAPTURE: {
      limitReached = limitType == MOTOR_LIMIT_MIN || limitType == MOTOR_LIMIT_HOME ? 
                    m_limitGripperValue <= 140 :
                    m_limitGripperValue >= 600;
    }
    break;
    default: break;
  }
  return limitReached;
}

void ApplicationArduino::enableEngine(bool enable) {
  this->printf("%s engine\r\n",enable?"ENABLE":"DISABLE");
  if(enable) {
    digitalWrite(enPin, LOW);
  } else {
    digitalWrite(enPin, HIGH);
  }
  
}

void ApplicationArduino::initDirection(int motorID, int direction)
{
  switch(motorID){
    case MOTOR::MOTOR_ARM1:
    case MOTOR::MOTOR_ARM2: {
      m_listStepper[motorID]->setDir(direction);
    }
    break;
    case MOTOR::MOTOR_ARM5:
    {
      m_listMiniStepper[MINISTEPPER_UPDOWN_ID]->setDir(direction);
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      m_listMiniStepper[MINISTEPPER_GRIPPER_ID]->setDir(direction);
    }
    break;
    default: break;
  }
  
}

void ApplicationArduino::moveStep(int motorID, int currentStep, int nextStep)
{
  switch(motorID){
    case MOTOR::MOTOR_ARM1: {
      m_listStepper[motorID]->moveStep(1000);
    }
    break;
    case MOTOR::MOTOR_ARM2: {
      m_listStepper[motorID]->moveStep(4000);
    }
    break;
    case MOTOR::MOTOR_ARM5:
    {
      m_listMiniStepper[MINISTEPPER_UPDOWN_ID]->moveStep(1000);
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      m_listMiniStepper[MINISTEPPER_GRIPPER_ID]->moveStep(1000);
    }
    break;
    default: break;
  }
}
