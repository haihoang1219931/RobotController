#include "ApplicationArduino.h"
#include "SmoothMotion.h"
#include <Arduino.h>
#include "MiniStepper_driver.h"
#include "Stepper_driver.h"
#include "SAL/Button.h"
#include "SAL/ChessBoard.h"
#include "SAL/Robot.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "SAL/StdTypes.h"

#define NONE_PIN  0xFF
#define enPin 8
#define stepXPin 2 //X.STEP
#define dirXPin 5 // X.DIR
#define stepYPin 4 //Y.STEP
#define dirYPin 7 // Y.DIR

#define limitX 13 // X.LIMIT
#define limitY 10 // Y.LIMIT

#define miniStepperUpdownPin1 34
#define miniStepperUpdownPin2 36
#define miniStepperUpdownPin3 38
#define miniStepperUpdownPin4 40

#define miniStepperGripperPin1 24
#define miniStepperGripperPin2 26
#define miniStepperGripperPin3 28
#define miniStepperGripperPin4 30

#define limitUpdown 11
#define limitGripper 62 // A8

Stepper_driver driver1(enPin,stepXPin, dirXPin);
Stepper_driver driver2(enPin,stepYPin, dirYPin);
MiniStepper_driver miniStepperUpdown(
      miniStepperUpdownPin1,miniStepperUpdownPin2,miniStepperUpdownPin3,miniStepperUpdownPin4);
MiniStepper_driver miniStepperGripper(
      miniStepperGripperPin1,miniStepperGripperPin2,miniStepperGripperPin3,miniStepperGripperPin4);

SmoothMotion motionDriver1(enPin,stepXPin, dirXPin);
SmoothMotion motionDriver2(enPin,stepYPin, dirYPin);
SmoothMotion motionUpdown(miniStepperUpdownPin1,miniStepperUpdownPin2,
  miniStepperUpdownPin3,miniStepperUpdownPin4);
SmoothMotion motionGripper(
      miniStepperGripperPin1,miniStepperGripperPin2,miniStepperGripperPin3,miniStepperGripperPin4);
ApplicationArduino::ApplicationArduino()
{
    initRobot();
    memset(m_buttonPin,NONE_PIN,sizeof(m_buttonPin));
    m_buttonPin[MOTOR_ARM1] = limitX;
    m_buttonPin[MOTOR_ARM2] = limitY;
    m_buttonPin[MOTOR_ARM5] = limitUpdown;

    pinMode(m_buttonPin[MOTOR_ARM1], INPUT_PULLUP);
    pinMode(m_buttonPin[MOTOR_ARM2], INPUT_PULLUP);
    pinMode(m_buttonPin[MOTOR_ARM5], INPUT_PULLUP);
    pinMode(enPin, OUTPUT);
    driver1.init();
    driver2.init();
    digitalWrite(enPin, HIGH);

    initHardwareTimer();
}

ApplicationArduino::~ApplicationArduino()
{

}

void ApplicationArduino::initRobot()
{
    m_chessBoard->setChessBoardPosX(31-31*8/2);
    m_chessBoard->setChessBoardPosY(100);
    m_chessBoard->setChessBoardSize(31*8);
    m_chessBoard->setDropZoneSpace(31);

    JointParam armPrams[MAX_MOTOR] = {
    // active|   scale=gear_ratio/resolution   |length|init angle|home angle|home step|min angle|max angle|
      {true,  1.0f/1.0f,                            0,     100,        0,        1,       0,       250   },
      {true,  8.0f*18.0f/01.0f*(200.0f/360.0f),   255,       0,      -17,      100,     -17,       150   },
      {true,  8.0f*70.0f/20.0f*(200.0f/360.0f),    85,     140,       50,      500,      50,       210   },
      {false,  1.0f/1.0f,                          15,     130,      130,        1,     130,       130   },
      {false,  1.0f/1.0f,                         120,     180,      180,        1,     180,       180   },
      {true,  50.0f/14.0f*(512.0f/360.0f),          0,      20,        0,     1000,       0,        45   }
    };

    for(int motor= MOTOR_CAPTURE; motor<= MOTOR_ARM5; motor++) {
        m_robot->setMotorParam(motor,armPrams[motor]);
    }
}

int ApplicationArduino::printf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    char m_buffer[128];
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
  // this->printf("Limit Gripper pin[%d] Value: %d\r\n", limitGripper, m_limitGripperValue);
}
#define DEBUG_SERIAL
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
                    m_limitGripperValue >= 500 :
                    m_limitGripperValue <= 200;
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
  this->printf("initDirection motorID=%d, direction=%d\r\n", motorID, direction);
  switch(motorID){
    case MOTOR::MOTOR_ARM1: {
      driver1.setDir(direction);
    }
    break;
    case MOTOR::MOTOR_ARM2: {
      driver2.setDir(direction);
    }
    break;
    case MOTOR::MOTOR_ARM5:
    {
      miniStepperUpdown.setDir(direction);
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      miniStepperGripper.setDir(direction);
    }
    break;
    default: break;
  }
  
}

void ApplicationArduino::moveSingleStep(int motorID, int delayTime)
{
  switch(motorID){
    case MOTOR::MOTOR_ARM1: {
      driver1.moveStep(delayTime);
    }
    break;
    case MOTOR::MOTOR_ARM2: {
      driver2.moveStep(delayTime);
    }
    break;
    case MOTOR::MOTOR_ARM5:
    {
      miniStepperUpdown.moveStep(delayTime);
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      miniStepperGripper.moveStep(delayTime);
    }
    break;
    default: break;
  }
}
void ApplicationArduino::moveDoneAction(int motorID)
{
  switch(motorID){
    case MOTOR::MOTOR_ARM1: {
      // Do nothing
    }
    break;
    case MOTOR::MOTOR_ARM2: {
      // Do nothing
    }
    break;
    case MOTOR::MOTOR_ARM5:
    {
      miniStepperUpdown.enable(false);
    }
    break;
    case MOTOR::MOTOR_CAPTURE: 
    {
      miniStepperGripper.enable(false);
    }
    break;
    default: break;
  }
}

void ApplicationArduino::initHardwareTimer()
{
  const float samplerate = 40000.0f; // 40KHz
  // initialize timer1
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 16000000.0f / samplerate; // compare match register for IRQ with selected samplerate
  TCCR1B |= (1 << WGM12); // CTC mode
  TCCR1B |= (1 << CS10); // no prescaler
  TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
  interrupts(); // enable all interrupts
}

ISR(TIMER1_COMPA_vect)
{
  motionDriver1.motionControlLoop();
  motionDriver2.motionControlLoop();
  motionUpdown.motionControlLoop();
  motionGripper.motionControlLoop();
}
