#ifndef APPLICATIONSIM_H
#define APPLICATIONSIM_H

#include "../SAL/ApplicationController.h"
class MainProcess;
class ApplicationSim: public ApplicationController
{
public:
    ApplicationSim(MainProcess* mainProcess);
    int printf(const char *fmt, ...) override;
    void msleep(int millis) override;
    long getSystemTime() override;
    void enableEngine(bool enable) override;
    bool isLimitReached(int motorID,
                            MOTOR_LIMIT_TYPE limitType) override;
    int readSerial(char* output, int length) override;
    int getMotorAngle(int motorID);
private:
    MainProcess* m_mainProcess;
    char m_command[MAX_COMMAND_LENGTH];
};

#endif // APPLICATIONSIM_H
