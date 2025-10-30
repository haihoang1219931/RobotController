#include "HardwareSerial.h"
/*
  DC_driver.cpp - Library to use DC motor drivers such as the L298N.
  Created by Csermendy Bendeguz, March 29, 2025.
*/

#include "Arduino.h"
#include "DC_driver.h"

DC_driver::DC_driver(byte in1, byte in2 , bool stopOnFinish)
{
  m_in1 = in1;
  m_in2 = in2;
  m_startTime = 0;
  m_state = DC_DONE;
  m_stopOnFinish = stopOnFinish;
}

void DC_driver::moveTo(int position) {
  Serial.println("moveTo:");
  m_targetPosition = position;
  m_state = DC_INIT;
  m_direction = m_targetPosition > m_currentPosition ? 1:-1;
}

void DC_driver::setCurrentPosition(int position) {
  m_currentPosition = position;
}

int DC_driver::currentPosition() {
  return m_currentPosition;
}

void DC_driver::setSpeed(float speed) {
  Serial.println("setSpeed:");
  m_speed = speed;
  m_state = DC_INIT;
}

float DC_driver::speed() {
  return m_speed;
}

bool DC_driver::isFinished() {
  Serial.println("isFinished:");
  return m_state == DC_DONE;
}

void DC_driver::setDir(int direction)
{
  pinMode(m_in1, OUTPUT);
  pinMode(m_in2, OUTPUT);
  m_direction = direction;
  digitalWrite(m_in1, LOW);
  digitalWrite(m_in2, LOW);
}

void DC_driver::moveStep(int sleepTime)
{
  Serial.println("moveStep:");
  digitalWrite(m_in1, m_direction > 0 ? HIGH:LOW);
  digitalWrite(m_in2, m_direction > 0 ? LOW:HIGH);
}

void DC_driver::runSpeed(bool checkHome) {
  if(checkHome) {
    digitalWrite(m_in1, HIGH);
    digitalWrite(m_in2, LOW);
    return;
  } 
  long currentTime = millis();
  // Serial.print("m_state:");
  // Serial.println(m_state);
  switch(m_state){
    case DC_INIT: {
      Serial.print("m_direction:");
      Serial.println(m_direction);
      pinMode(m_in1, OUTPUT);
      pinMode(m_in2, OUTPUT);
      digitalWrite(m_in1, m_direction > 0 ? HIGH:LOW);
      digitalWrite(m_in2, m_direction > 0 ? LOW:HIGH);
      m_startTime = currentTime;
      m_startPosition = m_currentPosition;
      m_state = DC_EXECUTE;
    }
    break;
    case DC_EXECUTE: {
      long timeElapsed = currentTime - m_startTime;
      long nextPos = (long)((float)m_startPosition + m_speed * m_direction * (float)timeElapsed/1000.0f);
      
      if(m_direction * nextPos <= m_direction * m_targetPosition) {
        m_currentPosition = nextPos;
      } else {
        digitalWrite(m_in1, LOW);
        digitalWrite(m_in2, LOW);
        m_state = DC_DONE;
        // Serial.println("DONE");
        // 
      }
      Serial.println(".");
      // Serial.print("m_speed:");
      // Serial.println(m_speed);
      // Serial.print("nextPos:");
      // Serial.println(nextPos);
      // Serial.print("timeElapsed:");
      // Serial.println(timeElapsed);
      // Serial.print("m_currentPosition:");
      // Serial.println(m_currentPosition);
      // Serial.print("m_targetPosition:");
      // Serial.println(m_targetPosition);
    }
    break;
    case DC_DONE: {

    }
    break;
    default: break;
  }
}

void DC_driver::stop() {
  digitalWrite(m_in1, LOW);
  digitalWrite(m_in2, LOW);
  m_state = DC_DONE;
}