#include "Stepper_driver.h"

Stepper_driver::Stepper_driver(byte enablePin, byte stepPin, byte dirPin):
  m_enablePin(enablePin),
  m_stepPin(stepPin),
  m_dirPin(dirPin)
{
  pinMode(m_enablePin, OUTPUT);
  pinMode(m_stepPin, OUTPUT);
  pinMode(m_dirPin, OUTPUT);
}

void Stepper_driver::enable(bool enableDriver)
{
  digitalWrite(m_enablePin, enableDriver? LOW:HIGH);
}

void Stepper_driver::setDir(byte dir)
{
  // dir > 0 means clockwise
  digitalWrite(m_dirPin, dir > 0 ? LOW:HIGH);
}

void Stepper_driver::moveStep(int sleepTime)
{
  digitalWrite(m_stepPin, HIGH);
  delayMicroseconds(sleepTime);
  digitalWrite(m_stepPin, LOW);
  delayMicroseconds(sleepTime);
}