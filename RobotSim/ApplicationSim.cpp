#include "ApplicationSim.h"
#include "../src/SAL/Motor.h"
#include "../src/SAL/Robot.h"
#include "../src/SAL/ChessBoard.h"
#include <stdio.h>
#include <stdarg.h>
#ifdef __linux__
#include <time.h>
#include <string.h>
#elif _WIN32
#include <time.h>
#include <windows.h>
#else
#endif
ApplicationSim::ApplicationSim(MainProcess* mainProcess):
    m_mainProcess(mainProcess)
{
    memset(m_command,0x00U, sizeof(m_command));
    initRobot();
}

ApplicationSim::~ApplicationSim()
{

}

void ApplicationSim::initRobot()
{
    m_chessBoard->setChessBoardPosX(13-13*8/2);
    m_chessBoard->setChessBoardPosY(46);
    m_chessBoard->setChessBoardSize(13*8);
    m_chessBoard->setDropZoneSpace(13);

    JointParam armPrams[MAX_MOTOR] = {
    // active |   scale   |   length   | init step | home angle | home step time |
        {true,  1.0f*1.0f,        0,         50,          0,             1,     },
        {true,  1.0f*1.0f,      115,         10,        -20,             1,     },
        {true,  1.0f*1.0f,       25,        150,         50,             1,     },
        {false, 1.0f*1.0f,       18,        130,        130,             1,     },
        {false, 1.0f*1.0f,       40,        180,        180,             1,     },
        {true,  1.0f*1.0f,       13,          1,         45,             1,     }
    };

    for(int motor= MOTOR_CAPTURE; motor<= MOTOR_ARM5; motor++) {
        m_robot->setMotorParam(motor,armPrams[motor]);
    }
}

void ApplicationSim::specificPlatformGohome(int motorID)
{
    //@todo: consider to optimize code    
}

void ApplicationSim::harwareStop(int motorID)
{
    //@todo: consider to optimize code
}

void ApplicationSim::checkInput()
{
    // Simulation do nothing here
}

int ApplicationSim::printf(const char *fmt, ...)
{
    char buffer[256];
    va_list args;
    va_start(args, fmt);
    int rc = vsprintf(buffer, fmt, args);
    va_end(args);
    ::printf("%s",buffer);
    fflush(stdout);
    return rc;
}

void ApplicationSim::msleep(int millis)
{
#ifdef __linux__
    //linux code goes here
    struct timespec ts = { millis / 1000, (millis % 1000) * 1000 * 1000 };
    nanosleep(&ts, NULL);
#elif _WIN32
    // windows code goes here
    Sleep(millis);
#else
#endif
}

long ApplicationSim::getSystemTime()
{
#ifdef USE_HARD_TIMER
#ifdef __linux__
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    return (curTime.tv_usec + curTime.tv_sec*1000000);
#elif _WIN32
    struct timeb curTime;
    ftime(&curTime);
    return (long)(1000*curTime.time + curTime.millitm);
#else
    return 0;
#endif
#else
    return m_appTimer;
#endif
}

bool ApplicationSim::isLimitReached(int motorID,
                        MOTOR_LIMIT_TYPE limitType)
{
    unsigned int maxStep[MAX_MOTOR];
    maxStep[MOTOR_ARM1] = 150;
    maxStep[MOTOR_ARM2] = 210;
    maxStep[MOTOR_ARM5] = 45;
#ifdef DEBUG_SIM
    printf("isLimitReached[%d] cur[%d] h[%d] dir[%d]\r\n",
           motorID,
           m_robot->currentStep(motorID),
           m_robot->homeStep(motorID),
           m_robot->dir(motorID));
#endif
    return limitType == MOTOR_LIMIT_MIN?
                (m_robot->currentStep(motorID) * m_robot->dir(motorID)
                 >= m_robot->homeStep(motorID) * m_robot->dir(motorID)):
                maxStep[motorID];
}

void ApplicationSim::enableEngine(bool enable)
{

}

int ApplicationSim::readSerial(char* output, int length)
{
    int commandLength = strlen(m_command);
    memcpy(output,m_command,strlen(m_command));
    memset(m_command,0x00U, sizeof(m_command));
    return commandLength;
}

void ApplicationSim::initDirection(int motorID, int direction)
{

}

void ApplicationSim::moveStep(int motorID, int currentStep, int nextStep)
{
//    int simCurrentStep = m_robot->currentStep(motorID);
//    int dir = m_robot->dir(motorID);
//    m_robot->setCurrentStep(motorID,simCurrentStep + dir);
#ifdef DEBUG_SIM
    this->printf("Sim M[%d] S[%d]\r\n",motorID,simCurrentStep + dir);
#endif
}

int ApplicationSim::getMotorAngle(int motorID)
{
    return m_robot->currentStep(motorID);
}

void ApplicationSim::simulateReceivedCommand(char* command, int length)
{
    memcpy(m_command,command,length);
}
