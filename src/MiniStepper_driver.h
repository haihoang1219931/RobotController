#ifndef MINISTEPPER_DRIVER_H
#define MINISTEPPER_DRIVER_H

#include "Arduino.h"

class MiniStepper_driver
{
  public:
    MiniStepper_driver(byte pin1, byte pin2, byte pin3, byte pin4);
    void enable(bool enableDriver);
    void setDir(int dir);
    void moveStep(int sleepTime);    
  private:
    byte m_pin1;
    byte m_pin2;
    byte m_pin3;
    byte m_pin4;
    int m_dir;
};

#endif

