#include <Arduino.h>
/*
  DC_driver.cpp - Library to use DC motor drivers such as the L298N.
  Created by Csermendy Bendeguz, March 29, 2025.
*/

#include "Servo_driver.h"

Servo_driver::Servo_driver(int pin)
{
  m_pin = pin;
  Serial.print("Servo_driver init pin:");
  Serial.println(m_pin);
  fastMoveToTarget(100);
  detach();
}

void Servo_driver::connect()
{
  attach(m_pin);
}

void Servo_driver::fastMoveToTarget(int position) {
  if(!attached()) attach(m_pin);
  write(position);
}

int Servo_driver::pin()
{
  return m_pin;
}