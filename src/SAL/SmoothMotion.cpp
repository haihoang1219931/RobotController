#include "SmoothMotion.h"
#include "Robot.h"
SmoothMotion::SmoothMotion(uint32_t id, Robot* robot):
  m_robot(robot),
  m_id(id),
  m_stateControl(MOTOR_EXECUTE_WAIT_COMMAND)
{
}

// void setAcc(float acceleration) {
//   if(acceleration <= 0){return;}
//   first_step_delay =1000*(sqrt(2*1.8)/acceleration);

//   Serial.print("Acceleration ->  ");
//   Serial.println(acceleration);
// }

void SmoothMotion::setupTarget(
  uint32_t stepsAccel, uint32_t stepsCruise, uint32_t stepsDecel, 
  int direction, bool isAccel,  uint32_t accelStartWaitPulse, uint32_t minWaitPulse) {
  m_numStepAccel = stepsAccel;
  m_numStepCruise = stepsCruise;
  m_numStepDecel = stepsDecel;
  m_targetDirection = direction;
  m_isAccel = isAccel;
  m_numWaitPulse = (float)accelStartWaitPulse;
  m_minWaitPulse = minWaitPulse;

  m_pulseCount = 0;
  m_stepCountAccel = 0;
  m_stepCountCruise = 0;
  m_stepCountDecel = 0;
  changeStateControl(m_isAccel? MOTOR_EXECUTE_INCREASE_SPEED : MOTOR_EXECUTE_CRUISE_SPEED);
}

float SmoothMotion::delayAccel(float stepCount, float delayCur) {
#ifndef DEBUG_COUNT_STEP
  float nextDelay = delayCur * (4.0f*stepCount - 1.0f) / (4.0f*stepCount + 1.0f);
  return nextDelay > m_minWaitPulse ? nextDelay: m_minWaitPulse;
#else
  return delayCur;
#endif
}

float SmoothMotion::delayDecel(float stepCount, float delayCur) {
#ifndef DEBUG_COUNT_STEP
  float nextDelay = delayCur * (4.0f*stepCount + 1.0f) / (4.0f*stepCount - 1.0f);
  return nextDelay;
#else
  return delayCur;
#endif
}

void SmoothMotion::changeStateControl(int newState)
{
  if(m_stateControl != newState) {
    m_stateControl = newState;
    if(m_stateControl != MOTOR_EXECUTE_DONE) {
      m_statePulse = STATE_COMMAND1;
    }
    // switch(m_stateControl){
    //   case MOTOR_EXECUTE_INCREASE_SPEED: Serial.println("Accel");
    //   break;
    //   case MOTOR_EXECUTE_CRUISE_SPEED: Serial.println("Cruise");
    //   break;
    //   case MOTOR_EXECUTE_DECREASE_SPEED: Serial.println("Decel");
    //   break;
    // }
  }
}

void SmoothMotion::motionControlLoop() {
  switch (m_stateControl) {
    case MOTOR_EXECUTE_INCREASE_SPEED:{
      increaseSpeed();
    }
    break;
    case MOTOR_EXECUTE_CRUISE_SPEED:{
      cruiseSpeed();
    }
    break;
    case MOTOR_EXECUTE_DECREASE_SPEED:{
      decreaseSpeed();
    }
    break;
    case MOTOR_EXECUTE_HOME: {
      goHome();
    }
    break;
    case MOTOR_EXECUTE_DONE: {
      m_stateControl = MOTOR_EXECUTE_WAIT_COMMAND;
    }
    break;
  }
}

void SmoothMotion::increaseSpeed() {
  m_statePulse = pulseLoop();
  if(m_statePulse != STATE_DONE) return;
  restartPulse();
  m_stepCountAccel ++;
  m_numWaitPulse = delayAccel(m_stepCountAccel,m_numWaitPulse);
  if(m_stepCountAccel >= m_numStepAccel){
    changeStateControl(MOTOR_EXECUTE_CRUISE_SPEED);
  }
}

void SmoothMotion::cruiseSpeed() {
  m_statePulse = pulseLoop();
  if(m_statePulse != STATE_DONE) return;
  restartPulse();
  m_stepCountCruise ++;
  if(m_stepCountCruise >= m_numStepCruise){
    changeStateControl(m_isAccel? MOTOR_EXECUTE_DECREASE_SPEED:MOTOR_EXECUTE_DONE);
  }
}

void SmoothMotion::decreaseSpeed() {
  m_statePulse = pulseLoop();
  if(m_statePulse != STATE_DONE) return;
  m_stepCountDecel ++;
  m_numWaitPulse = delayDecel(m_numStepDecel - m_stepCountDecel,m_numWaitPulse);  
  if(m_stepCountDecel >= m_numStepDecel) {
    changeStateControl(MOTOR_EXECUTE_DONE);
  }
}

void SmoothMotion::goHome()
{

}

uint8_t SmoothMotion::pulseLoop()
{
  return m_robot->pulseLoop((int)m_id);
}

void SmoothMotion::restartPulse()
{
  m_robot->updateCountPulse((int)m_id,0);
}

uint32_t SmoothMotion::getCurrentSteps()
{
  return m_stepCountAccel + m_stepCountCruise + m_stepCountDecel;
}
