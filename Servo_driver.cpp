#include "Arduino.h"
/*
  DC_driver.cpp - Library to use DC motor drivers such as the L298N.
  Created by Csermendy Bendeguz, March 29, 2025.
*/

#include "Servo_driver.h"

Servo_driver::Servo_driver(int pin)
{
  m_pin = pin;
}

void Servo_driver::moveTo(int position) {
  m_targetPosition = position;
  m_timerStart = false;
  m_direction = m_targetPosition > m_currentPosition ? 1:-1;
}

void Servo_driver::setSpeed(float speed) {
  m_speed = speed;
  m_timerStart = false;
}

void Servo_driver::setCurrentPosition(int position) {
  m_currentPosition = position;
}

int Servo_driver::currentPosition() {
  return m_currentPosition;
}

void Servo_driver::runSpeed() {
  long currentTime = millis();
  if(!attached()) attach(m_pin);
  if(!m_timerStart) {
    m_startTime = currentTime;
    m_timerStart = true;
    m_startPosition = m_currentPosition;
  }
  long timeElapsed = currentTime - m_startTime;
  long nextPos = (long)((float)m_startPosition + m_speed * (float)timeElapsed/1000.0f * (float)m_direction);
  

  if(m_direction * nextPos < m_direction * m_targetPosition) {
    // Serial.print("dir:");
    // Serial.println(m_direction);
    // Serial.print("timeElapsed:");
    // Serial.println((float)timeElapsed/1000.0f);
    // Serial.print("nextPos:");
    // Serial.println(nextPos);
    // Serial.print("m_currentPosition:");
    // Serial.println(m_currentPosition);
    // Serial.print("m_targetPosition:");
    // Serial.println(m_targetPosition);
    write((int)nextPos);
    m_currentPosition = nextPos;
  }
}
