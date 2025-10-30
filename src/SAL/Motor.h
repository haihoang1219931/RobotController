#ifndef MOTOR_H
#define MOTOR_H

#include "StdTypes.h"

class Robot;
class Motor
{
public:
    Motor(Robot* robot, int motorID = 0);
    void initPlan(int targetStep, int stepTime, int direction, bool isGoHome = false);
    void executePlan();
    void increaseSpeed();
    void cruiseSpeed();
    void decreaseSpeed();
    void goHome();
    int currentStep();
    int targetStep();
    bool isFinishExecution();
private:
    Robot* m_robot;
    int m_motorID;
    MOTOR_CONTROL_STATE m_state;
    int m_currStep;
    int m_startStep;
    int m_targetStep;
    int m_stepTime;
    int m_direction;
    bool m_stopOnFinish;
};

#endif // MOTOR_H
