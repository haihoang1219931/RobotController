#ifndef DC_driver_h
#define DC_driver_h

#include "Arduino.h"

enum DC_CONTROL_STATE{
  DC_INIT,
  DC_EXECUTE,
  DC_DONE
} ;
class DC_driver
{
  public:
    DC_driver(byte in1, byte in2);
    void setRuntime(int runtime);
    void setSpeed(float speed);
    void runSpeed();
    bool isFinished();
  private:
    uint8_t m_state;
    int m_runTime;
    float m_speed;
    long m_startTime;
    byte m_in1;
    byte m_in2;
    
};

#endif

