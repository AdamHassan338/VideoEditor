#ifndef TIMELINECONTROLLER_H
#define TIMELINECONTROLLER_H

#include <QObject>

#include <QObject>
#include <QMainWindow>
#include <QPixmap>
#include "GUI/timelinewidget.h"
#include "Core/videofile.h"
#include "qlabel.h"
#include "Core/videowriter.h"

class TimelineController : public QObject
{
    Q_OBJECT
public:
    explicit TimelineController(QObject *parent = nullptr);
    TimelineController(QMainWindow* mainwindow, TimelineWidget* timeline, QObject* parent = nullptr);


signals:
    uint8_t updateSignal(uint8_t* frameBuffer, int width, int height);
    uint8_t encodeSignal(uint8_t* frameBuffer);
    void updateBlankSignal();
public slots:
    //void onTimelinePositionChanged(int position);
    //void onVideoSelectionChanged(int videoIndex);
    //void onVideoFrameDecoded(QImage frame);
    void seekFrame(VideoFile* video,uint frameNumber);
    void updateRender(uint8_t* frameData, int width, int height);
    void updateRenderBlank();
    void startRender();

private:
    QMainWindow* m_mainwindow;
    TimelineWidget* m_timelineWidget;
    QLabel* m_pixmapLabel;
    //QMap<int, VideoFile*> m_videoFiles;
    VideoFile* m_currentVideoFile;
    uint m_currentFrame;
    videoWriter* encoder;

    void moveToStart();


};

#endif // TIMELINECONTROLLER_H
