#include "ApplicationArduino.h"
#include "Stepper_driver.h"
#include "Servo_driver.h"
#include "DC_driver.h"
#include "SAL/Button.h"
#include "SAL/ChessBoard.h"
#include "SAL/Robot.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "SAL/StdTypes.h"

#define NONE_PIN  0xFF
const int enPin=8;
const int stepXPin = 2; //X.STEP
const int dirXPin = 5; // X.DIR
const int stepYPin = 3; //Y.STEP
const int dirYPin = 6; // Y.DIR
const int stepZPin = 4; //Z.STEP
const int dirZPin = 7; // Z.DIR

const int limitX = 9;
const int limitY = 10;
const int limitZ = 11;

const int servoPin = 45;

const int dcIN1 = 42;
const int dcIN2 = 44;
volatile char m_buffer[128];
ApplicationArduino::ApplicationArduino()
{
    memset(m_buttonPin,NONE_PIN,sizeof(m_buttonPin));
    m_buttonPin[MOTOR_ARM1] = limitX;
    m_buttonPin[MOTOR_ARM2] = limitY;
    m_buttonPin[MOTOR_CAPTURE] = limitZ;

    pinMode(m_buttonPin[MOTOR_ARM1], INPUT_PULLUP);
    pinMode(m_buttonPin[MOTOR_ARM2], INPUT_PULLUP);
    pinMode(m_buttonPin[MOTOR_CAPTURE], INPUT_PULLUP);
    
    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, HIGH);

    m_servoDriver = new Servo_driver(servoPin);    
    m_dcDriver = new DC_driver(dcIN1, dcIN2);

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
        {true,  1.0f*1.0f,     0,    100,        0,        1,       0,       250   },
        {true,  14.0f*1.0f,  275,      0,        0,        1,       0,       150   },
        {true,  2.0f*1.0f,    55,    140,       00,        1,       0,       210   },
        {false, 1.0f*1.0f,    25,    130,      135,        1,       0,         0   },
        {false, 1.0f*1.0f, 32+35,    180,      135,        1,       0,         0   },
        {true,  1.0f*1.0f,    85,     20,        0,        1,       0,        45   }
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
  if(motorID == MAX_MOTOR || motorID == MOTOR::MOTOR_ARM5)
  m_servoDriver->fastMoveToTarget(100);
}

void ApplicationArduino::harwareStop(int motorID = MAX_MOTOR)
{
  if(motorID == MAX_MOTOR || motorID == MOTOR::MOTOR_CAPTURE)
  m_dcDriver->stop();
}

void ApplicationArduino::checkInput(){
	for(unsigned int btnID = 0; btnID< MAX_BUTTON; btnID++) {
    if(m_buttonPin[btnID] != NONE_PIN) {
      pinMode(m_buttonPin[btnID], INPUT_PULLUP);
      storeButtonState(btnID, digitalRead(m_buttonPin[btnID]) == LOW);
      // this->printf("button[%d] %s\r\n",btnID, digitalRead(m_buttonPin[btnID]) == LOW ? "pressed" : "normal");
    }
	}
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
                    m_servoDriver->read() == 100:
                    false;
      this->printf("APPArduino Servo p[%d] limitType[%d] limitReached[%s]\r\n",
        m_servoDriver->read(),limitType,
        limitReached?"true":"false"
       );
    }
    break;
    case MOTOR::MOTOR_CAPTURE: {
      limitReached = limitType == MOTOR_LIMIT_MIN || limitType == MOTOR_LIMIT_HOME ? 
                    (m_buttonList[MOTOR_CAPTURE]->buttonState() != BUTTON_STATE::BUTTON_NOMAL) :
                    m_robot->currentStep(motorID) >= m_robot->maxStep(motorID);
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
    m_servoDriver->connect();
  } else {
    digitalWrite(enPin, HIGH);
    m_servoDriver->detach();
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
      //@todo implement move step with Servo motor
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      //@todo implement move step with DC motor
      m_dcDriver->setDir(direction);
    }
    break;
    default: break;
  }
  
}

void ApplicationArduino::moveStep(int motorID, int currentStep, int nextStep)
{
  switch(motorID){
    case MOTOR::MOTOR_ARM1:
    case MOTOR::MOTOR_ARM2: {
      m_listStepper[motorID]->moveStep(500);
    }
    break;
    case MOTOR::MOTOR_ARM5:
    {
      //@todo implement move step with Servo motor
      m_servoDriver->fastMoveToTarget(m_robot->stepToAngle(motorID, 100-nextStep));
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      //@todo implement move step with DC motor
      m_dcDriver->moveStep(1000);
      this->printf("APPArduino M[%d] s[%d]\r\n",
        motorID,m_robot->currentStep(motorID));
    }
    break;
    default: break;
  }
}
