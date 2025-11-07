#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <Arduino.h>
#include <Servo.h>

class Servo_driver: public Servo
{
  public:
    Servo_driver(int pin);
    void connect();    
    void fastMoveToTarget(int position);
    int pin();
  private:
    int m_pin;
};

#endif

