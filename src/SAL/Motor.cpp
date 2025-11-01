#include "Motor.h"
#include "Robot.h"
#include <stdio.h>
#ifdef ARDUINO
#include "../ApplicationArduino.h"

#ifdef abs
#undef abs
#endif

#define abs(x) ((x)>0?(x):-(x))

extern ApplicationArduino* app;
#else
#include <math.h>
#endif
Motor::Motor(Robot* robot, int motorID):
    m_robot(robot),
    m_motorID(motorID),
    m_currStep(0),
    m_startStep(0),
    m_targetStep(0)
{
}
void Motor::initPlan(int targetStep, int stepTime, bool isRelativeMove)
{
    m_direction = targetStep>m_currStep?
                        1:(targetStep==m_currStep?0:-1);
    m_startStep = m_currStep;
    m_targetStep = isRelativeMove ? targetStep + m_currStep : targetStep;
    m_stepTime = stepTime;
    m_state = MOTOR_INIT;
    m_robot->initDirection(m_motorID,m_direction);
#ifdef ARDUINO
    app->printf("init[%d] tar[%d] stp[%d] dir[%d]\r\n",
           m_motorID, m_targetStep, m_stepTime, m_direction);
#else
    printf("init[%d] tar[%d]/cur[%d] stp[%d] dir[%d]\r\n",
            m_motorID,
            m_targetStep, m_currStep,
            m_stepTime, m_direction);
#endif
}

void Motor::initGoHome()
{
    m_stepTime = m_param.homeStepTime;
    m_state = MOTOR_EXECUTE_HOME;
    m_direction = m_param.homeStep < m_currStep ? -1:1;
    m_robot->initDirection(m_motorID, m_direction);
}

void Motor::executePlan()
{
#ifdef DEBUG_MOTOR
    app->printf("Motor[%d] S[%d]\r\n",m_motorID,m_state);
#else
//    printf("Motor[%d] S[%d]\r\n",m_motorID,m_state);
#endif
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
#ifdef DEBUG_MOTOR
        app->printf("M[%d] time[%ld] numStep[%d] m_currStep[%d]\r\n",
               m_motorID,
               m_robot->elapsedTime(),
               numStep,
               m_currStep
               );
#else

#endif               
        m_currStep += m_direction;
        m_robot->moveStep(m_motorID, 
                          m_currStep-m_direction,
                          m_currStep);
//        printf("M[%d] time[%ld] numStep[%d] m_currStep[%d] m_direction[%d] max[%s] min[%s]\r\n",
//                       m_motorID,
//                       m_robot->elapsedTime(),
//                       numStep,
//                       m_currStep,
//                        m_direction,
//                        m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MAX) ? "true":"false",
//                        m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MIN) ? "true":"false"
//                       );
    }
    if(m_currStep == m_targetStep
            || (m_direction > 0 && !m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MAX))
            || (m_direction < 0 && !m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MIN))
        )
    {
        m_state = MOTOR_EXECUTE_DECREASE_SPEED;
    }
#ifdef DEBUG_MOTOR    
    printf("M[%d] next state[%d]\r\n",m_motorID,m_state);
#endif
}

void Motor::decreaseSpeed()
{
    m_state = MOTOR_DONE;
}

void Motor::goHome()
{
#ifdef DEBUG_MOTOR
    printf("goHome M[%d] ST[%d] T[%ld]\r\n",
           m_motorID, m_stepTime, m_robot->elapsedTime());
#endif
    if(!m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MIN))
    {
        if(m_robot->elapsedTime() > m_stepTime) {
            m_currStep += m_direction;
            m_robot->moveStep(m_motorID, m_currStep , m_param.homeStep);
        }
    } else {
        m_currStep = m_param.homeStep;
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

void Motor::setParam(JointParam param)
{
    m_param = param;
    m_currStep = m_param.initStep;
}

bool Motor::isActive()
{
    return m_param.active;
}

float Motor::length()
{
    return m_param.length;
}

float Motor::currentAngle()
{
    return m_currStep * m_param.scale;
}

float Motor::angleToStep(int angle)
{
    return (float)angle * m_param.scale;
}

int Motor::stepToAngle(int step)
{
    return (int)((float)step / m_param.scale);
}

int Motor::homeStep()
{
    return m_param.homeStep;
}

void Motor::setCurrentStep(int step)
{
    m_currStep = step;
}

void Motor::setDir(int dir)
{
    m_direction = dir;
}

int Motor::dir()
{
    return m_direction;
}
