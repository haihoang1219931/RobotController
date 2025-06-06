#include "HardwareSerial.h"
#include "ApplicationArduino.h"
#include "Button.h"
#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <AccelStepper.h>
    #include <Servo.h>
    #include "DC_driver.h"
#else
    #include <WProgram.h>
#endif

#include <stdio.h>
#include <stdarg.h>

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

const int servoPin=13;

const int dcIN1=42;
const int dcIN2=44;
volatile char m_buffer[128];
ApplicationArduino::ApplicationArduino()
{
    Serial.begin(38400);
    this->printf("ApplicationArduino constructor\r\n");    

    m_machineState = MACHINE_STATE::MACHINE_INIT;
    setMachineState(MACHINE_STATE::MACHINE_WAIT_COMMAND);
    
    m_buttonPin[BTN_BASE] = limitX;
    m_buttonPin[BTN_ARM1] = limitY;
    m_buttonPin[BTN_ARM2] = limitZ;

    pinMode(m_buttonPin[BTN_BASE], INPUT_PULLUP);
    pinMode(m_buttonPin[BTN_ARM1], INPUT_PULLUP);
    
    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, HIGH);

    m_listStepper[MOTOR_BASE] = new AccelStepper(AccelStepper::DRIVER, stepXPin, dirXPin);
    m_listStepper[MOTOR_ARM1] = new AccelStepper(AccelStepper::DRIVER, stepYPin, dirYPin);

    m_servo = new Servo();
    m_dcDriver = new DC_driver(dcIN1, dcIN2, 0); 
    m_servo->attach(servoPin);
    m_servo->write(0);
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

long ApplicationArduino::getSystemTimeInMillis() {
	return millis();
}

void ApplicationArduino::updateButtonState(BUTTON_ID btnID, bool pressed) {
    m_listButton[btnID]->setPressed(pressed);
}

void ApplicationArduino::checkInput(){
	for(unsigned int btnID = BTN_BASE; btnID< BTN_MAX; btnID++) {
    pinMode(m_buttonPin[btnID], INPUT_PULLUP);
		updateButtonState(btnID, digitalRead(m_buttonPin[btnID]) == LOW);
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

void ApplicationArduino::setMaxSpeed(MOTOR motor, float speed) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      m_listStepper[motor]->setMaxSpeed(speed);
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      
    }
    break;
    case MOTOR::MOTOR_ARM3:{

    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
}

void ApplicationArduino::setSpeed(MOTOR motor, float speed) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      m_listStepper[motor]->setSpeed(speed);
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      
    }
    break;
    case MOTOR::MOTOR_ARM3:{
      m_dcDriver->analogMove(speed>0?true:false, speed);
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
  
}

void ApplicationArduino::setAcceleration(MOTOR motor, float acceleration) {
  m_listStepper[motor]->setAcceleration(acceleration);
}

void ApplicationArduino::setTargetPos(MOTOR motor, long target) {
  m_listStepper[motor]->moveTo(target);
}

long ApplicationArduino::distanceToGo(MOTOR motor) {
  return m_listStepper[motor]->distanceToGo();
}

void ApplicationArduino::run(MOTOR motor) {
  m_listStepper[motor]->run();
}

void ApplicationArduino::runSpeed(MOTOR motor) {
  m_listStepper[motor]->runSpeed();
}

void ApplicationArduino::setCurrentPosition(MOTOR motor, long position) {
  m_listStepper[motor]->setCurrentPosition(position);
}

long ApplicationArduino::currentPosition(MOTOR motor) {
  // this->printf("MOTOR[%d] [%ld]\r\n",motor,m_listStepper[motor]->currentPosition());
  return m_listStepper[motor]->currentPosition();
}

float ApplicationArduino::speed(MOTOR motor) {
  return m_listStepper[motor]->speed();
}
float ApplicationArduino::maxSpeed(MOTOR motor) {
  return m_listStepper[motor]->maxSpeed();
}

void ApplicationArduino::setServoAngle(int angle) {
  m_servo->write(angle);
}
void ApplicationArduino::enableStepper(bool enable) {
  this->printf("%s Stepper\r\n",enable?"ENABLE":"DISABLE");
  digitalWrite(enPin, enable?LOW:HIGH);
  setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
}
