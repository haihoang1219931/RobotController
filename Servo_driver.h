#ifndef SERVO_DRIVER_H
#define SERVO_DRIVER_H

#include <Servo.h>

enum SERVO_CONTROL_STATE{
  SERVO_INIT,
  SERVO_EXECUTE,
  SERVO_DONE
};

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
    void fastMoveToTarget(int position);
    bool isFinished();
  private:
    int m_state;
    int m_pin;
    int m_direction;
    int m_currentPosition;
    int m_targetPosition;
    float m_speed;
    long m_startTime;
    int m_startPosition;
};

#endif

