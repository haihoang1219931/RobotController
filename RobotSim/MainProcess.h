#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QList>
#include "ApplicationSim.h"
#include "VideoDisplay/VideoRender.h"
class MainProcess : public QThread
{
    Q_OBJECT
    Q_PROPERTY(float captureStep READ captureStep NOTIFY captureStepChanged)
    Q_PROPERTY(QVariantList listAngle READ listAngle NOTIFY listAngleChanged)
    Q_PROPERTY(QVariantList listArmLength READ listArmLength NOTIFY listArmLengthChanged)
    Q_PROPERTY(QVariantList chessBoardInfo READ chessBoardInfo NOTIFY chessBoardInfoChanged)
public:
    explicit MainProcess(QThread *parent = nullptr);
    ~MainProcess();

    float captureStep();
    QVariantList listAngle();
    QVariantList listArmLength();
    QVariantList chessBoardInfo();
    void syncRobot();

public Q_SLOTS:
    void setCaptureStep(float captureStep);
    void setListAngle(int motorID, float angle);
    void setListArmLength(int motorID, float length);
    void setChessBoardInfo(float posX, float posY, float size);

Q_SIGNALS:
    void readyToUpdate();
    void captureStepChanged();
    void listAngleChanged();
    void listArmLengthChanged();
    void chessBoardInfoChanged();

public Q_SLOTS:
    void run() override;
    void startService();
    void stopService();
    void setRender(VideoRender* render);
    void updateScreen();
    void pause(bool pause);
    void executeCommand(QString command);
    void setBlackSide(bool isBlack);
    void setOpponentMove(int startRow, int startCol, int stopRow, int stopCol, char promotePiece);
    void setNextMove(int startRow, int startCol, int stopRow, int stopCol, char promotePiece);
    void updateRobotStep();
    void changeSleepTime(int sleepTime);
private:
    unsigned char m_renderData[93110400]; // 1920x1080 I420
    bool m_stopped;
    int m_sleepTime;
    QMutex *m_mutex;
    QWaitCondition* m_pauseCond;
    QThread* m_thread;
    bool m_pause = false;
    ApplicationSim* m_application;
    VideoRender* m_render = nullptr;
    float m_captureStep;
    QVariantList m_listAngle;
    QVariantList m_listArmLength;
    QVariantList m_chessBoardInfo;
};

#endif // MAINPROCESS_H
