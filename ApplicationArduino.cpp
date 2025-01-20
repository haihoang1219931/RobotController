#include "ApplicationArduino.h"
#include "Button.h"
#if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
    #include <AccelStepper.h>
    #include <Servo.h>
#else
    #include <WProgram.h>
#endif

#include <stdio.h>
#include <stdarg.h>

ApplicationArduino::ApplicationArduino()
{
    Serial.begin(115200);
    this->printf("ApplicationArduino constructor\r\n");
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

    m_machineState = MACHINE_STATE::MACHINE_INIT;
    setMachineState(MACHINE_STATE::MACHINE_WAIT_COMMAND);
    
    m_buttonPin[BTN_BASE] = limitX;
    m_buttonPin[BTN_ARM1] = limitY;
    m_buttonPin[BTN_ARM2] = limitZ;

    pinMode(m_buttonPin[BTN_BASE], INPUT_PULLUP);
    pinMode(m_buttonPin[BTN_ARM1], INPUT_PULLUP);
    pinMode(m_buttonPin[BTN_ARM2], INPUT_PULLUP);
    

    m_listStepper[MOTOR_BASE] = new AccelStepper(AccelStepper::FULL2WIRE, stepXPin, dirXPin);
    m_listStepper[MOTOR_ARM1] = new AccelStepper(AccelStepper::FULL2WIRE, stepYPin, dirYPin);
    m_listStepper[MOTOR_ARM2] = new AccelStepper(AccelStepper::FULL2WIRE, stepZPin, dirZPin);

    pinMode(enPin, OUTPUT);
    digitalWrite(enPin, LOW);

    m_servo = new Servo();
    m_servo->attach(servoPin);
    m_servo->write(0);
}
int ApplicationArduino::printf(const char *fmt, ...) {
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    int rc = vsprintf(buffer, fmt, args);
    va_end(args);
    Serial.print(buffer);
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

int ApplicationArduino::readSerial(char* output, int length) {
  long start = this->getSystemTimeInMillis();
  String command = Serial.readString();// read the incoming data as string  
  command.toCharArray(output,command.length());
  if(length > command.length())
    output[command.length()] = 0;
  return command.length();
}

void ApplicationArduino::setMaxSpeed(MOTOR motor, float speed) {
  m_listStepper[motor]->setMaxSpeed(speed);
}

void ApplicationArduino::setSpeed(MOTOR motor, float speed) {
  m_listStepper[motor]->setSpeed(speed);
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
  return m_listStepper[motor]->currentPosition();
}

float ApplicationArduino::speed(MOTOR motor) {
  return m_listStepper[motor]->speed();
}

void ApplicationArduino::setServoAngle(int angle) {
  m_servo->write(angle);
}
