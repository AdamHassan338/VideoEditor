#include "timelinecontroller.h"

TimelineController::TimelineController(QObject *parent)
    : QObject{parent}
{

}

TimelineController::TimelineController(QMainWindow *mainwindow, TimelineWidget *timelineWidget, QObject *parent) : QObject{parent}
{
    m_mainwindow = mainwindow;
    m_timelineWidget = timelineWidget;
    encoder = new videoWriter();
    connect(m_timelineWidget,&TimelineWidget::seekFrame,this,&TimelineController::seekFrame);
    connect(m_timelineWidget,&TimelineWidget::emptyFrame,this,&TimelineController::updateRenderBlank);

    //connect(m_timelineWidget,&TimelineWidget::renderFrame,this,&TimelineController::seekFrame);
    connect(m_timelineWidget,&TimelineWidget::renderEmptyFrame,this,&TimelineController::updateRenderBlank);
}

void TimelineController::seekFrame(VideoFile *video, uint frameNumber)
{
    m_currentVideoFile = video;

    m_currentFrame = frameNumber;

    connect(&video->decoder(),&videoreader::newFrame,this,&TimelineController::updateRender);
    video->seekFrame(m_currentFrame);

}

void TimelineController::updateRender(uint8_t* frameData, int width, int height)
{


    //qDebug() << "controler update";
    if(m_timelineWidget->getRendering()){

        encoder->encodeFrame(frameData);
    }
    emit updateSignal(frameData, width, height);
}

void TimelineController::updateRenderBlank()
{
    emit updateBlankSignal();
}

void TimelineController::startRender()
{
    encoder->setup("test.mp4");
    encoder->initializeScaler();
    //loop now somehow
    //move playhead to start
    m_timelineWidget->moveToStart();
    m_timelineWidget->setRendering(true);
    while(m_timelineWidget->getRendering()){
        m_timelineWidget->render();
        m_timelineWidget->moveFrame();
    }
    encoder->endFile();
    qDebug() << "end";
    //render frame
    //send to encoder
    //move playhead 1frame
}

void TimelineController::moveToStart()
{

}
