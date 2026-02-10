#ifndef APPLICATIONARDUINO_H
#define APPLICATIONARDUINO_H

#include <Arduino.h>
#include "SAL/ApplicationController.h"

class Stepper_driver;
class Servo_driver;
class DC_driver;
class MiniStepper_driver;
class ApplicationArduino : public ApplicationController
{
public:
	ApplicationArduino();
  ~ApplicationArduino();
  void initRobot() override;
  void specificPlatformGohome(int motorID = MAX_MOTOR) override;
  void harwareStop(int motorID = MAX_MOTOR) override;
  void checkInput() override;
  int printf(const char *fmt, ...) override;
  void msleep(int millis) override;
  long getSystemTime() override;
  bool isLimitReached(int motor,
                      MOTOR_LIMIT_TYPE limitType) override;
  int readSerial(char* output, int length) override;
  void enableEngine(bool enable) override;
  void initDirection(int motorID, int direction) override;
  void moveStep(int motorID, int currentStep, int nextStep) override;
private:
	uint8_t m_buttonPin[MAX_BUTTON];
  uint8_t m_limitGripperValue;
  Stepper_driver* m_listStepper[2];
  MiniStepper_driver* m_listMiniStepper[2];
};

#endif // APPLICATIONARDUINO_H
