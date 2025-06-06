/*
  DC_driver.cpp - Library to use DC motor drivers such as the L298N.
  Created by Csermendy Bendeguz, March 29, 2025.
*/

#include "Arduino.h"
#include "DC_driver.h"

DC_driver::DC_driver(byte in1, byte in2, byte en)
{
  m_in1 = in1;
  m_in2 = in2;
  m_en = en;
}

void DC_driver::begin()
{
  pinMode(m_in1, OUTPUT);
  pinMode(m_in2, OUTPUT);
  pinMode(m_en, OUTPUT);
}

void DC_driver::runSpeed() {
  if(m_direction) {
     digitalWrite(m_in1, HIGH);
     digitalWrite(m_in2, LOW);
   } else{
     digitalWrite(m_in1, LOW);
     digitalWrite(m_in2, HIGH);
   }
  analogWrite(m_en, m_speed);
}

void DC_driver::analogMove(bool direction, byte speed)
{
  m_direction = direction;
  m_speed = speed;
}
