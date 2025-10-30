#ifndef DC_driver_h
#define DC_driver_h

#include "Arduino.h"

enum DC_CONTROL_STATE{
  DC_INIT,
  DC_EXECUTE,
  DC_DONE
};
class DC_driver
{
  public:
    DC_driver(byte in1, byte in2, bool stopOnFinish = false);
    void moveTo(int position);
    void setCurrentPosition(int position);
    int currentPosition();
    void setSpeed(float speed);
    float speed();
    void runSpeed(bool checkHome = false);
    bool isFinished();
    void stop();
  private:
    uint8_t m_state;
    int m_startPosition;
    int m_currentPosition;
    int m_targetPosition;
    int m_direction;
    float m_speed;
    long m_startTime;
    bool m_stopOnFinish;
    byte m_in1;
    byte m_in2;
    
};

#endif

