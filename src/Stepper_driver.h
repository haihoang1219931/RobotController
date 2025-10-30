#ifndef STEPPER_DRIVER_H
#define STEPPER_DRIVER_H

#include "Arduino.h"

class Stepper_driver
{
  public:
    Stepper_driver(byte enablePin, byte stepPin, byte dirPin);
    void enable(bool enableDriver);
    void setDir(byte dir);
    void moveStep(int sleepTime);    
  private:
    byte m_enablePin;
    byte m_stepPin;
    byte m_dirPin;
    
};

#endif

