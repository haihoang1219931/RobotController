#include "SmoothMotion.h"

SmoothMotion::SmoothMotion(int enablePin, int dirPin, int stepPin):
  m_enablePin(enablePin),
  m_dirPin(dirPin),
  m_stepPin1(stepPin),
  m_motorType(MOTOR_TYPE_STEPPER_2_WIRES)
{

}

SmoothMotion::SmoothMotion(int stepPin1, int stepPin2, int stepPin3, int stepPin4):
  m_stepPin1(stepPin1),
  m_stepPin2(stepPin2),
  m_stepPin3(stepPin3),
  m_stepPin4(stepPin4),
  m_motorType(MOTOR_TYPE_STEPPER_4_WIRES)
{

}

void SmoothMotion::setupTarget(int targetStep, int direction, bool isAccel, float delayStart) {
  m_targetStep = targetStep;
  m_targetDirection = direction;
  m_isAccel = isAccel;
  m_delayStart = delayStart;
  if(m_isAccel) {
    m_stateControl = MOTOR_EXECUTE_INCREASE_SPEED;
  } else {
    m_stateControl = MOTOR_EXECUTE_CRUISE_SPEED;
  }
  m_delayTimeStart = delayStart;

}

float SmoothMotion::delayAccel(float stepCount, float delayCur) {
  // Serial.print("d[");
  // Serial.print(delayCur);
  // Serial.print("]");
  // Serial.print(" s[");
  // Serial.print(stepCount);
  // Serial.print("] T:");
  return delayCur * (4.0f*stepCount - 1.0f) / (4.0f*stepCount + 1.0f);
}

float SmoothMotion::delayDecel(float stepCount, float delayCur) {
  // Serial.print("d[");
  // Serial.print(delayCur);
  // Serial.print("]");
  // Serial.print(" s[");
  // Serial.print(stepCount);
  // Serial.print("] T:");
  return delayCur * (4.0f*stepCount + 1.0f) / (4.0f*stepCount - 1.0f);
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
      Serial.println("Done");
      m_stateControl = MOTOR_EXECUTE_WAIT_COMMAND;
    }
    break;
  }
}

void SmoothMotion::increaseSpeed() {
  switch(m_motionAccelState) {
    case MOTION_INIT: {
      Serial.println("Accel");
      m_statePulse = STATE_HIGH;
      m_numWaitPulse = m_delayTimeStart;
      m_delayTime = m_delayTimeStart;
      m_motionAccelState = MOTION_PULSE;
    }
    break;
    case MOTION_PULSE: {
      pulseLoop();
      if(m_statePulse == STATE_DONE) {
        m_motionAccelState = MOTION_UPDATE_DELAY;
      }
    }
    break;
    case MOTION_UPDATE_DELAY: {
      m_stepCountAccel ++;        
      m_delayTime = delayAccel(m_stepCountAccel,m_delayTime);
      m_numWaitPulse = m_delayTime;
      if(m_stepCountAccel < m_numStepAccel) {
        m_statePulse = STATE_HIGH;
        m_motionAccelState = MOTION_PULSE;
      } else {
        m_motionAccelState = MOTION_DONE;
      }
    }
    break;
    case MOTION_DONE: {
      m_stateControl = MOTOR_EXECUTE_CRUISE_SPEED;
      Serial.print("stepCountAccel:");
      Serial.println(m_stepCountAccel);
    }
    break;
  }
}

void SmoothMotion::cruiseSpeed() {
  switch(m_motionCruiseState) {
    case MOTION_INIT: {
      Serial.println("Cruise");
      m_statePulse = STATE_HIGH;
      m_numWaitPulse = 2;
      m_motionCruiseState = MOTION_PULSE;
    }
    break;
    case MOTION_PULSE: {
      pulseLoop();
      if(m_statePulse == STATE_DONE) {
        m_motionCruiseState = MOTION_CHECK_COUNTER;
      }
    }
    break;
    case MOTION_CHECK_COUNTER: {
      m_stepCountCruise ++;              
      if(m_stepCountCruise < m_numStepCruise) {
        m_statePulse = STATE_HIGH;
        m_motionCruiseState = MOTION_PULSE;
      } else {
        m_motionCruiseState = MOTION_DONE;
      }
    }
    break;
    case MOTION_DONE: {
      if(m_isAccel) {
        m_stateControl = MOTOR_EXECUTE_DECREASE_SPEED;
      } else {
        m_stateControl = MOTOR_EXECUTE_DONE;
      }
    }
    break;
  }
}

void SmoothMotion::decreaseSpeed() {
  switch(m_motionDecelState) {
    case MOTION_INIT: {
      Serial.println("Decel");
      m_statePulse = STATE_HIGH;
      m_numWaitPulse = 2;
      m_delayTime = 2; 
      m_stepCountDecel = m_numStepDecel;
      m_motionDecelState = MOTION_PULSE;
    }
    break;
    case MOTION_PULSE: {
      pulseLoop();
      if(m_statePulse == STATE_DONE) {
        m_motionDecelState = MOTION_UPDATE_DELAY;
      }
    }
    break;
    case MOTION_UPDATE_DELAY: {
      m_stepCountDecel --;        
      m_delayTime = delayDecel(m_stepCountDecel,m_delayTime);
      m_numWaitPulse = m_delayTime;
      if(m_stepCountDecel > 0) {
        m_statePulse = STATE_HIGH;
        m_motionDecelState = MOTION_PULSE;
      } else {
        m_motionDecelState = MOTION_DONE;
      }
    }
    break;
    case MOTION_DONE: {
      m_stateControl = MOTOR_EXECUTE_DONE;
    }
    break;
  }
}

void SmoothMotion::goHome()
{

}

void SmoothMotion::pulseLoop()
{
  if(m_motorType == MOTOR_TYPE_STEPPER_2_WIRES) {
    switch(m_statePulse){
      case STATE_HIGH: {
        digitalWrite(m_stepPin1,HIGH);
        m_statePulse = STATE_WAIT1;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT1: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_LOW;
      }
      break;
      case STATE_LOW: {
        digitalWrite(m_stepPin1,LOW);
        m_statePulse = STATE_WAIT2;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT2: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_DONE;
      }
      break;
    }
  } else if(m_motorType == MOTOR_TYPE_STEPPER_4_WIRES) {
    switch(m_statePulse){
      case STATE_COMMAND1: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? HIGH:HIGH);
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? LOW :HIGH);
        m_statePulse = STATE_WAIT1;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT1: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_COMMAND2;
      }
      break;
      case STATE_COMMAND2: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? HIGH:LOW );
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? HIGH:LOW );
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? LOW :HIGH);
        m_statePulse = STATE_WAIT2;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT2: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_COMMAND3;
      }
      break;
      case STATE_COMMAND3: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? HIGH:LOW );
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? LOW :HIGH);
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? LOW :HIGH);
        m_statePulse = STATE_WAIT3;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT3: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_COMMAND4;
      }
      break;
      case STATE_COMMAND4: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? HIGH:LOW );
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? HIGH:HIGH);
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? LOW :LOW );
        m_statePulse = STATE_WAIT4;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT4: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_COMMAND5;
      }
      break;
      case STATE_COMMAND5: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? LOW :HIGH);
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? HIGH:HIGH);
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? LOW :LOW );
        m_statePulse = STATE_WAIT5;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT5: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_COMMAND6;
      }
      break;
      case STATE_COMMAND6: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? LOW :HIGH);
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? HIGH:LOW );
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? HIGH:LOW );
        m_statePulse = STATE_WAIT6;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT6: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_COMMAND7;
      }
      break;
      case STATE_COMMAND7: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? LOW :HIGH);
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? LOW :HIGH);
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? HIGH:LOW );
        m_statePulse = STATE_WAIT7;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT7: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_COMMAND8;
      }
      break;
      case STATE_COMMAND8: {
        digitalWrite(m_stepPin1, m_targetDirection < 0 ? HIGH:HIGH);
        digitalWrite(m_stepPin2, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin3, m_targetDirection < 0 ? LOW :LOW );
        digitalWrite(m_stepPin4, m_targetDirection < 0 ? HIGH:LOW );
        m_statePulse = STATE_WAIT8;
        m_pulseCount = 0;
      }
      break;
      case STATE_WAIT8: {
        m_pulseCount++;
        if(m_pulseCount >= m_numWaitPulse) m_statePulse = STATE_DONE;
      }
      break;
    }
  } else {
    m_statePulse = STATE_DONE;
  }
}
