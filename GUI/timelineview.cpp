#include "GUI\timelineview.h"
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QFileInfo>
#include <QMimeData>
#include <QScrollBar>
#include "GUI\cliprect.h"
#include "GUI\common.h"
TimelineView::TimelineView(QWidget *parent)
    : QGraphicsView{parent}
{
    setAcceptDrops(true);
    m_scrollTimer.setInterval(100);
    connect(&m_scrollTimer, &QTimer::timeout, this, &TimelineView::onScrollTimerTimeout);
}

    //mouse handling

void TimelineView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsView::mousePressEvent(event);
}

void TimelineView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsView::mouseReleaseEvent(event);
}

void TimelineView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
}

    // drag and drop

bool isVideoFile(const QString& filePath)
{
    QFileInfo fileInfo(filePath);
    QString suffix = fileInfo.suffix().toLower();
    QStringList supportedVideoFormats = {"mp4", "mkv", "mov"}; // Add more video formats as needed

    return supportedVideoFormats.contains(suffix);
}

void TimelineView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        bool hasVideoFiles = false;
        QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl& url : urls)
        {
            if (isVideoFile(url.toLocalFile()))
            {
                hasVideoFiles = true;
                break;
            }
        }

        if (hasVideoFiles)
        {
            event->acceptProposedAction();
        }
    }

}

void TimelineView::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        m_lastDragPos = event->position().toPoint();
        event->acceptProposedAction();
        startAutoScroll();
    }
}

void TimelineView::dragLeaveEvent(QDragLeaveEvent *event)
{
    QGraphicsView::dragLeaveEvent(event);
}

void TimelineView::dropEvent(QDropEvent *event)
{

    if (event->mimeData()->hasUrls())
    {
        stopAutoScroll();
        QPointF mousePos = mapToScene(event->position().toPoint());
        qreal trackNum;
        //put clip in a track
        if(!scene()->sceneRect().contains(mousePos)){//if outside of scene
            mousePos = QPointF(mousePos.x(),scene()->sceneRect().height()-trackHeight-1);
            trackNum = mousePos.y()/40;
        }else{
            qreal dy = std::fmod(mousePos.y(), trackHeight);
            trackNum = mousePos.y()/trackHeight;


            mousePos.setY((trackNum*trackHeight)-dy);
            qDebug() << mousePos;
        }


        QList<QUrl> urls = event->mimeData()->urls();
        for (const QUrl& url : urls)
        {
            //add a graphics item
            qDebug() << mousePos;
            //emmit signal to video editor
            emit newClip(url.toLocalFile(),800,30,trackNum,mousePos.x());
            qDebug() << "Dropped file:" << url.toLocalFile();
        }
        event->acceptProposedAction();
    }

}
// auto scrolling

void TimelineView::startAutoScroll()
{
    if (!m_scrollTimer.isActive())
    {
        m_scrollTimer.start();
    }
}

void TimelineView::stopAutoScroll()
{
    if (m_scrollTimer.isActive())
    {
        m_scrollTimer.stop();
    }
}

void TimelineView::scrollIfNecessary()
{
    const int margin = 50; //scroll activation area
    const int stepSize = 10;
    int dx = 0;
    int dy = 0;

    if (m_lastDragPos.x() < margin)
    {
        dx = -stepSize;
    }
    else if (m_lastDragPos.x() > width() - margin)
    {
        dx = stepSize;
    }

    if (m_lastDragPos.y() < margin)
    {
        dy = -stepSize;
    }
    else if (m_lastDragPos.y() > height() - margin)
    {
        dy = stepSize;
    }

    if (dx != 0 || dy != 0)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + dx);
        verticalScrollBar()->setValue(verticalScrollBar()->value() + dy);
    }
}

void TimelineView::onScrollTimerTimeout()
{
    scrollIfNecessary();
}
