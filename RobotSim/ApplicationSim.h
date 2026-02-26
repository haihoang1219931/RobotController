#ifndef APPLICATIONSIM_H
#define APPLICATIONSIM_H

#include "../src/SAL/ApplicationController.h"
class MainProcess;
class ApplicationSim: public ApplicationController
{
public:
    ApplicationSim(MainProcess* mainProcess);
    ~ApplicationSim();
    void initRobot() override;
    void specificPlatformGohome(int motorID = MAX_MOTOR) override;
    void harwareStop(int motorID = MAX_MOTOR) override;
    void checkInput() override;
    int printf(const char *fmt, ...) override;
    void msleep(int millis) override;
    long getSystemTime() override;
    void enableEngine(bool enable) override;
    bool isLimitReached(int motorID,
                            MOTOR_LIMIT_TYPE limitType) override;
    int readSerial(char* output, int length) override;
    void initDirection(int motorID, int direction) override ;
    void moveSingleStep(int motorID, int delayTime) override;
    void moveDoneAction(int motorID) override;
    int getMotorAngle(int motorID);
    void simulateReceivedCommand(char* command, int length);
    uint8_t executePulseLoop(int motorID) override;
private:
    MainProcess* m_mainProcess;
    char m_command[MAX_COMMAND_LENGTH];    
};

#endif // APPLICATIONSIM_H
