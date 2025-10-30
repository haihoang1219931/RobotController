#ifndef APPLICATIONARDUINO_H
#define APPLICATIONARDUINO_H

#include <Arduino.h>
#include "SAL/ApplicationController.h"

typedef enum {
  MOTOR_ARM1,
  MOTOR_ARM2,
  MOTOR_ARM3,
  MOTOR_CAPTURE
} MOTOR;
class Stepper_driver;
class Servo_driver;
class DC_driver;
class ApplicationArduino : public ApplicationController
{
public:
	ApplicationArduino();
  ~ApplicationArduino();
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
  Stepper_driver* m_listStepper[2];
  Servo_driver* m_servoDriver;
  DC_driver* m_dcDriver;
};

#endif // APPLICATIONARDUINO_H
