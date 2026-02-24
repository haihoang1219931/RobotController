#ifndef SMOOTHMOTION_H
#define SMOOTHMOTION_H
#include <Arduino.h>
#include "SAL/StdTypes.h"
class SmoothMotion {
  public:
    SmoothMotion(int enablePin, int dirPin, int stepPin);
    SmoothMotion(int stepPin1, int stepPin2, int stepPin3, int stepPin4);

    void init();
    void setupTarget(int stepsAccel, int stepsCruise, int stepsDecel, 
      int direction, bool isAccel, float delayStart);
    void motionControlLoop();
    void pulseLoop();
    void increaseSpeed();
    void cruiseSpeed();
    void decreaseSpeed();
    void goHome();
    float delayAccel(float stepCount, float delayCur);
    float delayDecel(float stepCount, float delayCur);
    int getCurrentSteps();
  private:
    uint8_t m_statePulse;
    uint8_t m_numWaitPulse = 2;
    uint8_t m_pulseCount = 0;

    float m_delayTimeStart;
    float m_delayTime;
    uint8_t m_stateControl;
    
    uint32_t m_numStepAccel;
    uint32_t m_numStepCruise;
    uint32_t m_numStepDecel;

    uint32_t m_stepCountAccel;
    uint32_t m_stepCountCruise;
    uint32_t m_stepCountDecel;

    uint8_t m_motionAccelState;
    uint8_t m_motionCruiseState;    
    uint8_t m_motionDecelState;
    
    int m_targetStep;
    int m_targetDirection;
    bool m_isAccel;
    float m_delayStart;

    // pin numbers
    MOTOR_TYPE m_motorType;
    int m_enablePin;
    int m_dirPin;
    int m_stepPin1;
    int m_stepPin2;
    int m_stepPin3;
    int m_stepPin4;
};

#endif // SMOOTHMOTION_H