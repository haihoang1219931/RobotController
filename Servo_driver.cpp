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
  m_state = SERVO_INIT;
  m_direction = m_targetPosition > m_currentPosition ? 1:-1;
}

void Servo_driver::setSpeed(float speed) {
  m_speed = speed;
  m_state = SERVO_INIT;
}

float Servo_driver::speed(){
  return m_speed;
}

void Servo_driver::fastMoveToTarget(int position) {
  if(!attached()) attach(m_pin);
  write(position);
  m_state = SERVO_DONE;

}

void Servo_driver::setCurrentPosition(int position) {
  m_currentPosition = position;
}

int Servo_driver::currentPosition() {
  return m_currentPosition;
}

bool Servo_driver::isFinished() {
  return m_state == SERVO_DONE;
}

void Servo_driver::runSpeed() {
  long currentTime = millis();
  switch(m_state) {
    case SERVO_INIT:{
      if(!attached()) attach(m_pin);
      m_startTime = currentTime;
      m_startPosition = m_currentPosition;
      m_state = SERVO_EXECUTE;
    }
    break;
    case SERVO_EXECUTE:{
      long timeElapsed = currentTime - m_startTime;
      long nextPos = (long)((float)m_startPosition + m_speed * (float)timeElapsed/1000.0f);
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
      } else {
        m_state = SERVO_DONE;
      }
    }
    break;
    case SERVO_DONE:{

    }
    break;
    default: break;
  }
}
