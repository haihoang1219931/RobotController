#include "HardwareSerial.h"
/*
  DC_driver.cpp - Library to use DC motor drivers such as the L298N.
  Created by Csermendy Bendeguz, March 29, 2025.
*/

#include "Arduino.h"
#include "DC_driver.h"

DC_driver::DC_driver(byte in1, byte in2)
{
  m_in1 = in1;
  m_in2 = in2;
}

void DC_driver::setRuntime(int runTime) {
  m_runTime = runTime;
  m_state = DC_INIT;
}

void DC_driver::setSpeed(float speed) {
  m_speed = speed;
  m_state = DC_INIT;
}

bool DC_driver::isFinished() {
  return m_state == DC_DONE;
}

void DC_driver::runSpeed() {
  switch(m_state){
    case DC_INIT: {
      m_startTime = millis();
      pinMode(m_in1, OUTPUT);
      pinMode(m_in2, OUTPUT);
      digitalWrite(m_in1, m_speed > 0 ? HIGH:LOW);
      analogWrite(m_in2, (int)m_speed);
      m_state = DC_EXECUTE;
    }
    break;
    case DC_EXECUTE: {
      long currentTime = millis();
      if(currentTime - m_startTime > m_runTime) {
        digitalWrite(m_in1, LOW);
	      digitalWrite(m_in2, LOW);
        m_state = DC_DONE;
      }
    }
    break;
    case DC_DONE: {

    }
    break;
    default: break;
  }
}