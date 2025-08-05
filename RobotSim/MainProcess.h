#ifndef MAINPROCESS_H
#define MAINPROCESS_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include "ApplicationSim.h"
#include "VideoDisplay/VideoRender.h"
class MainProcess : public QThread
{
    Q_OBJECT
public:
    explicit MainProcess(QThread *parent = nullptr);
    ~MainProcess();

Q_SIGNALS:
    void readyToUpdate();
    void robotChanged(float angle1, float angle2, float angle3, float angle4);

public Q_SLOTS:
    void run() override;
    void startService();
    void stopService();
    void setRender(VideoRender* render);
    void updateScreen();
    void pause(bool pause);
    void setBlackSide(bool isBlack);
    void setOpponentMove(int startRow, int startCol, int stopRow, int stopCol, char promotePiece);
    void setNextMove(int startRow, int startCol, int stopRow, int stopCol, char promotePiece);
private:
    unsigned char m_renderData[93110400]; // 1920x1080 I420
    bool m_stopped;
    QMutex *m_mutex;
    QWaitCondition* m_pauseCond;
    QThread* m_thread;
    bool m_pause = false;
    ApplicationSim* m_application;
    VideoRender* m_render = nullptr;

};

#endif // MAINPROCESS_H
