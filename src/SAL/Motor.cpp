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
    m_direction = angleToStep(m_param.homeAngle) < m_currStep ? -1:
                  angleToStep(m_param.homeAngle) > m_currStep ? 1 : 0;
    m_robot->initDirection(m_motorID, m_direction);
}

void Motor::executePlan()
{
#ifdef DEBUG_MOTOR
    app->printf("Motor[%d] S[%d]\r\n",m_motorID,m_state);
#else
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
    if(m_currStep == m_targetStep
            || (m_direction > 0 && m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MAX))
            || (m_direction < 0 && m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_MIN))
        )
    {
        m_state = MOTOR_EXECUTE_DECREASE_SPEED;
    } else {   
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
//            printf("M[%d] time[%ld] numStep[%d] curr[%d] dir[%d]\r\n",
//                   m_motorID,
//                   m_robot->elapsedTime(),
//                   numStep,
//                   m_currStep,
//                   m_direction
//                   );
    #endif               
            m_currStep += m_direction;
            m_robot->moveStep(m_motorID, 
                            m_currStep-m_direction,
                            m_currStep);
        }
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
//#ifdef DEBUG_MOTOR
    app->printf("goHome M[%d] ST[%d] T[%ld]\r\n",
           m_motorID, m_stepTime, m_robot->elapsedTime());
//#endif
    if(!m_robot->isLimitReached(m_motorID,MOTOR_LIMIT_HOME))
    {
        if(m_robot->elapsedTime() > m_stepTime) {
            m_currStep += m_direction;
            m_robot->moveStep(m_motorID, m_currStep , angleToStep(m_param.homeAngle));
        }
    } else {
        m_currStep = angleToStep(m_param.homeAngle);
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
    m_currStep = angleToStep(m_param.initAngle);
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
    return m_currStep / m_param.scale;
}

int Motor::angleToStep(float angle)
{
    return (int)(angle * m_param.scale);
}

float Motor::stepToAngle(int step)
{
    return (float)step / m_param.scale;
}

int Motor::homeStep()
{
    return angleToStep(m_param.homeAngle);
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

int Motor::state()
{
    return m_state;
}

int Motor::minStep()
{
    return angleToStep(m_param.minAngle);
}

int Motor::maxStep()
{
    return angleToStep(m_param.maxAngle);
}

float Motor::homeAngle()
{
    return m_param.homeAngle;
}
