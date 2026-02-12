#ifndef APPLICATIONARDUINO_H
#define APPLICATIONARDUINO_H

#include <Arduino.h>
#include "SAL/ApplicationController.h"

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
  void moveDoneAction(int motorID) override;
private:
  uint8_t m_buttonPin[MAX_BUTTON];
  int m_limitGripperValue;
};

#endif // APPLICATIONARDUINO_H
