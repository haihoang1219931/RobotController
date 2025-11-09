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

//#define ROBOT_SENSE
void ApplicationSim::initRobot()
{
#ifdef ROBOT_SENSE
    m_chessBoard->setChessBoardPosX(13-13*8/2);
    m_chessBoard->setChessBoardPosY(46);
    m_chessBoard->setChessBoardSize(13*8);
    m_chessBoard->setDropZoneSpace(13);

    JointParam armPrams[MAX_MOTOR] = {
    // active |   scale   |length|init angle|home angle|home step|min angle|max angle|
         {true,  1.0f*1.0f,     0,      50,        0,        1,       0,       100   },
         {true,   1.0f*1.0f,   115,      0,      -20,        1,     -20,       150   },
         {true,  1.0f*1.0f,    25,    140,       50,        1,      50,       210   },
         {false,  1.0f*1.0f,    18,    130,      130,        1,       0,         0   },
         {false,  1.0f*1.0f,    40,    180,      180,        1,       0,         0   },
         {true,  1.0f*1.0f,    13,     20,       45,        1,       0,        45   }
    };
#else
    m_chessBoard->setChessBoardPosX(13-13*8/2);
    m_chessBoard->setChessBoardPosY(46);
    m_chessBoard->setChessBoardSize(13*8);
    m_chessBoard->setDropZoneSpace(13);

    JointParam armPrams[MAX_MOTOR] = {
    // active |   scale   |length|init angle|home angle|home step|min angle|max angle|
        {true,  1.0f*1.0f,     0,    100,        0,        1,       0,       250   },
        {true,  1.0f*1.0f,  275/2,      0,        0,        1,       0,       150   },
        {true,  1.0f*1.0f,    35/2,    90,       90,        1,       0,       410   },
        {false, 1.0f*1.0f,    25/2,    135,      135,        1,       0,         0   },
        {false, 1.0f*1.0f, (32+35)/2,    135,      135,        1,       0,         0   },
        {true,  1.0f*1.0f,    85/2,     20,        0,        1,       0,        45   }
    };
#endif
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
    bool result = false;

    if(limitType == MOTOR_LIMIT_MIN)
        result = m_robot->currentStep(motorID)
                == m_robot->minStep(motorID);
    else if(limitType == MOTOR_LIMIT_MAX)
        result = m_robot->currentStep(motorID)
                == m_robot->maxStep(motorID);
    else result = m_robot->currentStep(motorID)
            == m_robot->homeStep(motorID);
#ifdef DEBUG_SIM
    char strLimit[3][8] = {
        {"MIN"},
        {"MAX"},
        {"HOME"}
    };
    printf("SIM M[%d] limit[%s][%s] home[%d] min[%d] cur[%d] max[%d] dir[%d]\r\n",
           motorID,
           strLimit[limitType],
           result?"true":"false",
           m_robot->homeStep(motorID),
           m_robot->minStep(motorID),
           m_robot->currentStep(motorID),
           m_robot->maxStep(motorID),
           m_robot->dir(motorID));
#endif
    return result;
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
