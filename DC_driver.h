/*
  DC_driver.h - Library to use DC motor drivers such as the L298N.
  Created by Csermendy Bendeguz, March 29, 2025.
*/

#ifndef DC_driver_h
#define DC_driver_h

#include "Arduino.h"

class DC_driver
{
  public:
    DC_driver(byte in1, byte in2, byte en);
    void begin();
    void runSpeed();
    void analogMove(bool direction, byte speed);
  private:
    byte m_in1;
    byte m_in2;
    byte m_en;
    bool m_direction;
    byte m_speed;
};

#endif

