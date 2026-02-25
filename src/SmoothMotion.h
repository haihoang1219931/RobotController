#ifndef SMOOTHMOTION_H
#define SMOOTHMOTION_H
#include <Arduino.h>
#include "SAL/StdTypes.h"

class SmoothMotion {
  public:
    SmoothMotion(uint32_t id, int enablePin, int dirPin, int stepPin);
    SmoothMotion(uint32_t id, int stepPin1, int stepPin2, int stepPin3, int stepPin4);

    void init();
    void setupTarget(uint32_t stepsAccel, uint32_t stepsCruise, uint32_t stepsDecel, 
      int direction, bool isAccel, uint32_t accelStartWaitPulse, uint32_t minWaitPulse);
    void motionControlLoop();
    void pulseLoop();
    void increaseSpeed();
    void cruiseSpeed();
    void decreaseSpeed();
    void goHome();
    float delayAccel(float stepCount, float delayCur);
    float delayDecel(float stepCount, float delayCur);
    uint32_t getCurrentSteps();
    void changeStateControl(int newState);
    

  public:
    uint32_t m_totalPulse;
    
    uint32_t m_id;
    uint32_t m_minWaitPulse;
    
    float m_numWaitPulse;
    uint32_t m_pulseCount;

    
    uint32_t m_numStepAccel;
    uint32_t m_numStepCruise;
    uint32_t m_numStepDecel;

    uint32_t m_stepCountAccel;
    uint32_t m_stepCountCruise;
    uint32_t m_stepCountDecel;

    // uint8_t m_motionAccelState;
    // uint8_t m_motionCruiseState;    
    // uint8_t m_motionDecelState;
    
    int m_targetStep;
    int m_targetDirection;
    

    // pin numbers
    
    int m_enablePin;
    int m_dirPin;
    int m_stepPin1;
    int m_stepPin2;
    int m_stepPin3;
    int m_stepPin4;

    int m_motorType;
    bool m_isAccel;
    bool m_enableLogPulse;
    uint8_t m_statePulse;
    uint8_t m_stateControl;
};

#endif // SMOOTHMOTION_H