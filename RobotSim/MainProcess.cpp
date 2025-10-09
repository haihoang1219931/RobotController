#include "MainProcess.h"
#include "ApplicationSim.h"
MainProcess::MainProcess(QThread *parent) :
    QThread(parent),
    m_stopped(false),
    m_sleepTime(30),
    m_thread(nullptr)
{
    m_application = new ApplicationSim(this);
    memset(m_renderData,0,sizeof(m_renderData));
    m_mutex = new QMutex;
    m_pauseCond = new QWaitCondition;
    for(int i=0; i< MAX_MOTOR; i++)
    {
        m_listAngle.append(0);
        m_listArmLength.append(0);
    }
    for(int i=0; i< MAX_PARAMS_CHESSBOARD; i++)
    {
        m_chessBoardInfo.append(0);
    }
    syncRobot();
}

MainProcess::~MainProcess()
{
    stopService();
    sleep(2);
}


void MainProcess::pause(bool pause){
    if(pause == true){
        m_mutex->lock();
        m_pause = true;
        m_mutex->unlock();
    }else{
        m_mutex->lock();
        m_pause = false;
        m_mutex->unlock();
        m_pauseCond->wakeAll();
    }
}
void MainProcess::setBlackSide(bool isBlack) {
//    m_application->chessController()->setChessPiece(isBlack);
}

void MainProcess::setOpponentMove(int startRow, int startCol, int stopRow, int stopCol, char promotePiece) {
//    ChessMove chessMove = {startRow,startCol,stopRow,stopCol,false,false,promotePiece};
//    m_application->chessController()->setOpponentMove(chessMove);
}

void MainProcess::setNextMove(int startRow, int startCol, int stopRow, int stopCol, char promotePiece) {
//    ChessMove chessMove = {startRow,startCol,stopRow,stopCol,false,false,promotePiece};
//    m_application->chessController()->setNextMove(chessMove);
}

void MainProcess::syncRobot()
{
    float listArmLength[MAX_MOTOR];
    int numArm;
    m_application->getCurrentArmLength(listArmLength,&numArm);
    for(int i=0; i< numArm; i++)
    {
        m_listArmLength[i] = listArmLength[i];
        printf("m_listArmLength[%d] %f\r\n",
               i,m_listArmLength[i].toFloat());
    }

    float listAngle[MAX_MOTOR];
    float captureStep;
    int numMotor;
    printf("numMotor[%d]\r\n",numMotor);
    m_application->getCurrentPosition(listAngle,&numMotor,&captureStep);
    for(int i=0; i< numMotor; i++)
    {
        m_listAngle[i] = listAngle[i];
        printf("m_listAngle[%d] %f\r\n",
               i,m_listAngle[i].toFloat());
    }

    float chessBoardInfo[MAX_PARAMS_CHESSBOARD];
    int numParam;
    m_application->getChessBoardParams(chessBoardInfo,&numParam);
    for(int i=0; i< numParam; i++)
    {
        m_chessBoardInfo[i] = chessBoardInfo[i];
        printf("chessBoardInfo[%d] %f\r\n",
               i,m_chessBoardInfo[i].toFloat());
    }
    Q_EMIT listArmLengthChanged();
    Q_EMIT listAngleChanged();
    Q_EMIT chessBoardInfoChanged();
}

void MainProcess::run() {
    m_application->printf("Start\r\n");
    while(!m_stopped) {
        m_mutex->lock();
        if(m_pause)
            m_pauseCond->wait(m_mutex); // in this place, your thread will stop to execute until someone calls resume
        m_mutex->unlock();
        m_application->loop();
        m_application->msleep(m_sleepTime);
        updateRobotStep();
    }
    m_application->printf("Exit\r\n");
}
void MainProcess::startService() {
    if(m_thread != nullptr)
        m_thread->start();
    else start();
}

void MainProcess::stopService() {
    m_stopped = true;
    m_thread->terminate();
}
void MainProcess::setRender(VideoRender* render)
{
    m_render = render;
}
void MainProcess::updateScreen() {

}
void MainProcess::executeCommand(QString command)
{
    m_application->executeCommand((char*)command.toStdString().c_str());
}

void MainProcess::updateRobotStep()
{
    float listSteps[MAX_MOTOR];
    float captureStep;
    int numMotor;
    for(int i=0; i< MAX_MOTOR; i++){
        listSteps[i] = 0;
    }
    m_application->getCurrentPosition(listSteps,&numMotor,&captureStep);
    for(int i=0; i< numMotor; i++){
        setListAngle(i,listSteps[i]);
    }
    setCaptureStep(captureStep);
    Q_EMIT listAngleChanged();
    Q_EMIT listArmLengthChanged();
}

void MainProcess::changeSleepTime(int sleepTime)
{
    m_sleepTime = sleepTime;
}

float MainProcess::captureStep()
{
    return m_captureStep;
}

QVariantList MainProcess::listAngle()
{
    return m_listAngle;
}

QVariantList MainProcess::listArmLength()
{
    return m_listArmLength;
}

QVariantList MainProcess::chessBoardInfo()
{
    return m_chessBoardInfo;
}

void MainProcess::setCaptureStep(float captureStep)
{
    m_captureStep = captureStep;
    Q_EMIT captureStepChanged();
}

void MainProcess::setListAngle(int motorID, float angle)
{
    m_listAngle[motorID] = angle;
}

void MainProcess::setListArmLength(int motorID, float length)
{
    m_listArmLength[motorID] = length;
}

void MainProcess::setChessBoardInfo(float posX, float posY, float size)
{
    m_chessBoardInfo[0] = posX;
    m_chessBoardInfo[1] = posY;
    m_chessBoardInfo[2] = size;
    Q_EMIT chessBoardInfoChanged();
}
