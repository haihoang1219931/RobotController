#include "ApplicationArduino.h"
#include "Stepper_driver.h"
#include "Servo_driver.h"
#include "DC_driver.h"
#include "SAL/Button.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

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
}

ApplicationArduino::~ApplicationArduino()
{

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

void ApplicationArduino::hardwareGohome(int motorID)
{
  if(motorID == MAX_MOTOR || motorID == MOTOR::MOTOR_ARM3)
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
// #define READBYTE
int ApplicationArduino::readSerial(char* output, int length) {
  // long start = this->getSystemTimeInMillis();
#ifndef READBYTE
  String command = Serial.readString();// read the incoming data as string
  if(command.length()>0) {
    Serial.print(command);
    // command.getBytes(output,command.length(),0);
    for(int i=0; i< command.length(); i++) {
      output[i] = command.charAt(i);
      Serial.print(output[i],HEX);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
  return command.length();
#else
  int numBytes = Serial.readBytes(output,length);
  if(numBytes>0) {
    for(int i=0; i< numBytes; i++) {
      Serial.print(output[i],HEX);
      Serial.print(" ");
    }
    Serial.print("\n");
  }
  return numBytes;
#endif
}

bool ApplicationArduino::isLimitReached(int motorID, MOTOR_LIMIT_TYPE limitType)
                      {
  bool motorHomed = true;
  switch(motorID){
    case MOTOR::MOTOR_ARM1: {
      motorHomed = limitType == MOTOR_LIMIT_MIN ? 
                    (m_buttonList[MOTOR_ARM1]->buttonState() != BUTTON_STATE::BUTTON_NOMAL) :
                    false;
    }
    break;
    case MOTOR::MOTOR_ARM2: {
      motorHomed = limitType == MOTOR_LIMIT_MIN ? 
                    (m_buttonList[MOTOR_ARM2]->buttonState() != BUTTON_STATE::BUTTON_NOMAL) :
                    false;
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      motorHomed = limitType == MOTOR_LIMIT_MIN ? 
                    m_servoDriver->currentPosition() == 100:
                    false;
    }
    break;
    case MOTOR::MOTOR_CAPTURE: {
      motorHomed = limitType == MOTOR_LIMIT_MIN ? 
                    (m_buttonList[MOTOR_CAPTURE]->buttonState() != BUTTON_STATE::BUTTON_NOMAL) :
                    false;
    }
    break;
    default: break;
  }
  return motorHomed;
}

void ApplicationArduino::enableEngine(bool enable) {
  this->printf("%s Stepper\r\n",enable?"ENABLE":"DISABLE");
  digitalWrite(enPin, enable?LOW:HIGH);
}

void ApplicationArduino::initDirection(int motorID, int direction)
{
  switch(motorID){
    case MOTOR::MOTOR_ARM1:
    case MOTOR::MOTOR_ARM2: {
      m_listStepper[motorID]->setDir(direction);
    }
    break;
    case MOTOR::MOTOR_ARM3:
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
    case MOTOR::MOTOR_ARM3:
    {
      //@todo implement move step with Servo motor
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      //@todo implement move step with DC motor
      m_dcDriver->moveStep(1000);
    }
    break;
    default: break;
  }
}
