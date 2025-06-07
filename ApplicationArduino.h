#ifndef APPLICATIONARDUINO_H
#define APPLICATIONARDUINO_H

#include "ApplicationController.h"

class AccelStepper;
class Servo_driver;
class DC_driver;
class ApplicationArduino : public ApplicationController
{
public:
	ApplicationArduino();
    int printf(const char *fmt, ...) override;
    void msleep(int millis) override;
    long getSystemTimeInMillis() override;
    int readSerial(char* output, int length) override;
    void updateButtonState(BUTTON_ID btnID, bool pressed);
    void checkInput();
    void setMaxSpeed(MOTOR motor, float speed) override;
    void setSpeed(MOTOR motor, float speed) override;
    void setAcceleration(MOTOR motor, float acceleration) override;
    void setTargetPos(MOTOR motor, long target) override;
    long distanceToGo(MOTOR motor) override;
    void run(MOTOR motor) override;
    void runSpeed(MOTOR motor) override;
    void setCurrentPosition(MOTOR motor, long position) override;
    long currentPosition(MOTOR motor) override;
    float speed(MOTOR motor) override;
    float maxSpeed(MOTOR motor) override;
    void enableEngine(bool enable) override;
private:
	int m_buttonPin[BUTTON_ID::BTN_MAX];
  AccelStepper* m_listStepper[2];
  Servo_driver* m_servoDriver;
  DC_driver* m_dcDriver;
};

#endif // APPLICATIONARDUINO_H
