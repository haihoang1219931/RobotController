#include "ApplicationSim.h"
#include "../src/SAL/Motor.h"
#include "../src/SAL/Robot.h"
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
}

ApplicationSim::~ApplicationSim()
{

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
//    memset(maxStep,0xFF,sizeof(maxStep)*sizeof(unsigned int));
    maxStep[0] = 150;
    maxStep[1] = 50;
    maxStep[2] = 25;
    return limitType == MOTOR_LIMIT_MIN?
                m_robot->getMotor(motorID)->currentStep()<=0 :
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

}

int ApplicationSim::getMotorAngle(int motorID)
{
    return m_robot->getMotor(motorID)->currentStep();
}

void ApplicationSim::simulateReceivedCommand(char* command, int length)
{
    memcpy(m_command,command,length);
}
