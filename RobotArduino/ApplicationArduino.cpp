#include "HardwareSerial.h"
#include "ApplicationArduino.h"
#include "Button.h"
#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <AccelStepper.h>
    #include "Servo_driver.h"
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

const int servoPin = 45;

const int dcIN1 = 42;
const int dcIN2 = 44;
volatile char m_buffer[128];
ApplicationArduino::ApplicationArduino()
{
    Serial.begin(38400);
    this->printf("ApplicationArduino constructor\r\n");    

    m_machineState = MACHINE_STATE::MACHINE_INIT;
    setMachineState(MACHINE_STATE::MACHINE_WAIT_COMMAND);
    
    m_buttonPin[BTN_BASE] = limitX;
    m_buttonPin[BTN_ARM1] = limitY;
    m_buttonPin[BTN_ARM3] = limitZ;

    pinMode(m_buttonPin[BTN_BASE], INPUT_PULLUP);
    pinMode(m_buttonPin[BTN_ARM1], INPUT_PULLUP);
    pinMode(m_buttonPin[BTN_ARM3], INPUT_PULLUP);
    
    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, HIGH);

    m_listStepper[MOTOR_BASE] = new AccelStepper(AccelStepper::DRIVER, stepXPin, dirXPin);
    m_listStepper[MOTOR_ARM1] = new AccelStepper(AccelStepper::DRIVER, stepYPin, dirYPin);

    m_servoDriver = new Servo_driver(servoPin);
    m_dcDriver = new DC_driver(dcIN1, dcIN2);
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
    // this->printf("button[%d] %s\r\n",btnID, digitalRead(m_buttonPin[btnID]) == LOW ? "pressed" : "normal");
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
      m_servoDriver->setSpeed(speed);
    }
    break;
    case MOTOR::MOTOR_ARM3:{
      m_dcDriver->setSpeed(speed);
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
}

void ApplicationArduino::setAcceleration(MOTOR motor, float acceleration) {
  // m_listStepper[motor]->setAcceleration(acceleration);
}

void ApplicationArduino::setTargetPos(MOTOR motor, long target) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      m_listStepper[motor]->moveTo(target);
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      m_servoDriver->moveTo(target);
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      m_dcDriver->moveTo(target);
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
}

bool ApplicationArduino::isMoveDone(MOTOR motor) {
  bool moveDone = false;
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      moveDone = m_listStepper[motor]->distanceToGo() == 0;
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      moveDone = m_servoDriver->isFinished();
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      moveDone = m_dcDriver->isFinished();
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
  return moveDone;
}

void ApplicationArduino::run(MOTOR motor) {
  // m_listStepper[motor]->run();
}

void ApplicationArduino::runSpeed(MOTOR motor) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      m_listStepper[motor]->runSpeed();
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      m_servoDriver->runSpeed();
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      m_dcDriver->runSpeed();
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
}

void ApplicationArduino::setCurrentPosition(MOTOR motor, long position) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      m_listStepper[motor]->setCurrentPosition(position);
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      m_servoDriver->setCurrentPosition(position);
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      m_dcDriver->currentPosition();
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
}

long ApplicationArduino::currentPosition(MOTOR motor) {
  long currentPos = 0;
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      currentPos = m_listStepper[motor]->currentPosition();
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      currentPos = m_servoDriver->currentPosition();
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      currentPos = m_dcDriver->currentPosition();
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
  return currentPos;
}

void ApplicationArduino::goHome(MOTOR motor) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      m_listStepper[motor]->runSpeed();
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      m_servoDriver->fastMoveToTarget(100);
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      m_dcDriver->runSpeed(true);
    }
    break;
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
}

float ApplicationArduino::speed(MOTOR motor) {
  float speed = 0;
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      speed = m_listStepper[motor]->speed();
    }
    break;
    case MOTOR::MOTOR_ARM2:{
      speed = m_servoDriver->speed();
    }
    break;
    case MOTOR::MOTOR_ARM3:
    case MOTOR::MOTOR_MAX: break;
    default: break;
  }
  return speed;
}
float ApplicationArduino::maxSpeed(MOTOR motor) {
  float maxSpeed = 0;
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1: {
      maxSpeed = m_listStepper[motor]->maxSpeed();
    }
    break;
    case MOTOR::MOTOR_ARM2:
    case MOTOR::MOTOR_ARM3:
    case MOTOR::MOTOR_MAX:
    default: break;
  }
  return maxSpeed;
}

bool ApplicationArduino::isMotorHomed(MOTOR motor) {
  bool motorHomed = true;
  switch(motor){
    case MOTOR::MOTOR_BASE: {
      motorHomed = buttonState(BTN_BASE) != BUTTON_STATE::BUTTON_NOMAL;
    }
    break;
    case MOTOR::MOTOR_ARM1: {
      motorHomed = buttonState(BTN_ARM1) != BUTTON_STATE::BUTTON_NOMAL;
    }
    break;
    case MOTOR::MOTOR_ARM2: {
      motorHomed = m_servoDriver->isFinished();
    }
    break;
    case MOTOR::MOTOR_ARM3: {
      motorHomed = buttonState(BTN_ARM3) != BUTTON_STATE::BUTTON_NOMAL;
    }
    break;
    case MOTOR::MOTOR_MAX:
    default: break;
  }
  return motorHomed;
}

void ApplicationArduino::setHomePosition(MOTOR motor) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1:
      m_listStepper[motor]->setCurrentPosition(0);
    break;
    case MOTOR::MOTOR_ARM2: {
      m_servoDriver->setCurrentPosition(100);
    }
    break;
    case MOTOR::MOTOR_ARM3: {      
      m_dcDriver->setCurrentPosition(0);
      m_dcDriver->moveTo(5000);
    }
    break;
    case MOTOR::MOTOR_MAX:
    default: break;
  }
}

void ApplicationArduino::enableEngine(bool enable) {
  this->printf("%s Stepper\r\n",enable?"ENABLE":"DISABLE");
  digitalWrite(enPin, enable?LOW:HIGH);
  setMachineState(MACHINE_EXECUTE_COMMAND_DONE);
}

void ApplicationArduino::stop(MOTOR motor) {
  switch(motor){
    case MOTOR::MOTOR_BASE:
    case MOTOR::MOTOR_ARM1:
    case MOTOR::MOTOR_ARM2:
    break;
    case MOTOR::MOTOR_ARM3: {      
      m_dcDriver->stop();
    }
    break;
    case MOTOR::MOTOR_MAX:
    default: break;
  }
}
