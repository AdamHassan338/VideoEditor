#include "timelinecontroller.h"

TimelineController::TimelineController(QObject *parent)
    : QObject{parent}
{

}

TimelineController::TimelineController(QMainWindow *mainwindow, TimelineWidget *timelineWidget, QObject *parent)
{
    m_mainwindow = mainwindow;
    m_timelineWidget = timelineWidget;
    connect(m_timelineWidget,&TimelineWidget::seekFrame,this,&TimelineController::seekFrame);
    connect(m_timelineWidget,&TimelineWidget::emptyFrame,this,&TimelineController::updateRenderBlank);
}

void TimelineController::seekFrame(VideoFile *video, uint frameNumber)
{
    m_currentVideoFile = video;

    m_currentFrame = frameNumber;
    int a =2;
    connect(&video->decoder(),&videoreader::newFrame,this,&TimelineController::updateRender);
    video->seekFrame(m_currentFrame);
}

void TimelineController::updateRender(uint8_t* frameData, int width, int height)
{
    //qDebug() << "controler update";
    emit updateSignal(frameData, width, height);
}

void TimelineController::updateRenderBlank()
{
    emit updateBlankSignal();
}
