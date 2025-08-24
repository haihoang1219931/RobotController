#include "Motor.h"
#include "Robot.h"
#include <stdio.h>
#include <math.h>
Motor::Motor(Robot* robot, int motorID):
    m_robot(robot),
    m_motorID(motorID),
    m_currStep(0),
    m_startStep(0)
{
}
void Motor::initPlan(int targetStep, int stepTime, int direction, bool isGoHome)
{
    m_direction = isGoHome?-1:(targetStep>m_currStep?1:-1);
    m_startStep = m_currStep;
    m_targetStep = targetStep;
    m_stepTime = stepTime;
    m_state = isGoHome? MOTOR_EXECUTE_HOME:MOTOR_INIT;
    printf("init[%d] tar[%d] stp[%d] dir[%d]\r\n",
           m_motorID, m_targetStep, m_stepTime, m_direction);
}

void Motor::executePlan()
{
    printf("Motor[%d] S[%d]\r\n",m_motorID,m_state);
    switch (m_state) {
    case MOTOR_INIT: {
        // @Todo: init motor from application
        m_state = MOTOR_EXECUTE_INCREASE_SPEED;
    }
        break;
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
    case MOTOR_DONE: break;
    default: break;
    }
}

void Motor::increaseSpeed()
{
    m_state = MOTOR_EXECUTE_CRUISE_SPEED;
}

void Motor::cruiseSpeed()
{
    int numStep = m_robot->elapsedTime() / m_stepTime;
    if(numStep > abs(m_currStep - m_startStep))
    {
        printf("M[%d] time[%ld] numStep[%d] m_currStep[%d]\r\n",
               m_motorID,
               m_robot->elapsedTime(),
               numStep,
               m_currStep
               );
        m_currStep += m_direction;
        m_robot->moveStep(m_motorID);
    }
    if(m_currStep == m_targetStep
//            || (m_direction > 0 && !m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MAX))
//            || (m_direction < 0 && !m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MIN))
        )
    {
        m_state = MOTOR_EXECUTE_DECREASE_SPEED;
    }
    printf("M[%d] next state[%d]\r\n",m_motorID,m_state);
}

void Motor::decreaseSpeed()
{
    m_state = MOTOR_DONE;
}

void Motor::goHome()
{
    if(!m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MIN))
    {
        if(m_robot->elapsedTime() > m_stepTime)
        m_robot->moveStep(m_motorID);
    } else {
        m_state = MOTOR_DONE;
    }
}

int Motor::currentStep()
{
    return m_currStep;
}

int Motor::targetStep()
{
    return m_targetStep;
}

bool Motor::isFinishExecution()
{
    return m_state == MOTOR_DONE;
}
