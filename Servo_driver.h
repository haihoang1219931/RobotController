#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <Servo.h>

class Servo_driver: public Servo
{
  public:
    Servo_driver(int pin);
    void moveTo(int position);
    void setSpeed(float speed);
    float speed();
    void setCurrentPosition(int position);
    int currentPosition();
    void runSpeed();
  private:
    int m_pin;
    int m_direction;
    int m_currentPosition;
    int m_targetPosition;
    float m_speed;
    long m_startTime;
    int m_startPosition;
    bool m_timerStart;
};

#endif

