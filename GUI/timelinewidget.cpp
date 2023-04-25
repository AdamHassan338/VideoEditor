#include "GUI\timelinewidget.h"
#include "ui_timelinewidget.h"
#include "GUI\clipitemhandler.h"
#include "GUI\playheadhandler.h"
#include <QMouseEvent>
#include <QRectF>
#include <QDebug>
#include "GUI\common.h"
#include <vector>
#include "GUI\trackrect.h"

ClipItemHandler* ClipRect::handler = new ClipItemHandler();
PlayheadHandler* Playhead::handler = new PlayheadHandler();

ClipRect* highestClip(std::vector<ClipRect*> clips){//return clip on highest track

    ClipRect* top = nullptr;
    if(clips.empty()){
        return top;
    }
    top = clips[0];
    for(ClipRect* clip : clips){
        if(clip->pos().y() < top->pos().y()){
            top = clip;
        }
    }
    return top;
}

TimelineWidget::TimelineWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimelineWidget)
{
    //trackHeight = 40;
    //clipHeight= 40;

    ui->setupUi(this);
    scene = new TimelineScene(this);
    view = new TimelineView(this);
    view->frameRect();
    view->setScene(scene);
    view->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    scene->setSceneRect(view->rect());



    //scene->setSceneRect(view->rect());
    //scene->setSceneRect(scene->sceneRect());


    ui->verticalLayout->addWidget(view);
    view->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    //ClipRect* clip = new ClipRect();

    //clip->setRect(0,0,300,clipHeight);
    //clip->setPos(0,0);
    //clipRects.push_back(clip);
    //scene->addItem(clip);

    connect(ClipRect::handler,&ClipItemHandler::newTrack,this,&TimelineWidget::addTrack);
    connect(view,&TimelineView::newClip,this,&TimelineWidget::addClip);
    connect(Playhead::handler,&PlayheadHandler::playheadMoved,this,&TimelineWidget::underPlayhead);



    TrackRect* track = new TrackRect(0,0,scene->width()-1,trackHeight);
    trackRects.push_back(track);
    scene->addItem(track);



    playhead = new Playhead(scene->height()-1);
    scene->addItem(playhead);
    playhead->setPos(10,0);


}

double TimelineWidget::currentTime()
{
    return playheadPos / scaleFactor;
}

int TimelineWidget::currentFrame()
{
    return static_cast<int>(currentTime() * frameRate);
}

void TimelineWidget::underPlayhead()
{
    //qDebug() << "underplayhead";
    ClipRect* lastItem;
    std::vector<ClipRect*> clips;
    int count = 0;
    for(ClipRect *item : clipRects){
        if(item->collidesWithItem(playhead, Qt::IntersectsItemBoundingRect)){
            qDebug() << item->getName();
            clips.push_back(item);
            count++;
        }
    }
    qDebug() << count <<" clips underplayhead";

    lastItem = highestClip(clips);
    if(lastItem){
        qDebug() << "Frame: "<<getFrame(lastItem) <<"Of "<<lastItem->getName();

    }else{
        qDebug() << "no clips";
        emit emptyFrame();
    }

}

int TimelineWidget::getFrame(ClipRect* clip)
{
    //playheadpos
    QPointF clipStart = clip->pos();
    qreal playheadPos = playhead->pos().x();
    //being - clip pos
    qreal begin = clipStart.x();
    //  clip here
    //end // clip width + pos
    qreal end = begin + clip->rect().width();

    qreal clipWidth = clip->rect().width();

    qreal framepercent = (playheadPos- begin)/clipWidth;

    qreal frame = framepercent* clip->frames();
    frame = (float)(playheadPos- begin)* (float)(clip->frames() -1)/(float)clipWidth +1;

    int f = static_cast<int>(frame);

    if(f>clip->frames()){
        playhead->setX(playhead->x()+0.5);
        return -1;
        //return clip->frames();
    }

    //clip.getvideo
    //video.seekframe
    //run deoder
    //deocder emits the frame to video object,
    //video object returns it to clip.



    emit seekFrame(clip->video(),f);


    return f;
}

bool TimelineWidget::getRendering()
{
    return rendering;
}

void TimelineWidget::setRendering(bool newRendering)
{
    rendering = newRendering;

}

void TimelineWidget::moveToStart()
{
    playhead->setPos(1,0);
}

void TimelineWidget::moveFrame()
{
    if(playhead->pos().x()< scene->itemsBoundingRect().width() -1.0){
        playhead->moveBy(1.0,0.0);
    }else{
        rendering = false;
    }

}

void TimelineWidget::render()
{
    underPlayhead();
}

void TimelineWidget::addClip(QString filename,int frames,double framerate, int track, int x)
{
    Q_UNUSED(frames);Q_UNUSED(framerate);
    ClipRect* clip = new ClipRect(filename);
    double length = (clip->frames()-1) * scaleFactor /clip->framerate();
    clip->setFramerate(clip->framerate());
    clip->setFrames(clip->frames());
    clip->setRect(0,0,length,clipHeight);
    clip->setTrack(track);
    clip->setPos(x,trackHeight*track);
    scene->addItem(clip);
    clipRects.push_back(clip);


}

void TimelineWidget::addTrack(int x)
{
    Q_UNUSED(x);
    TrackRect* track = new TrackRect(0,0,scene->width()-1,trackHeight);
    track->setPos(0,trackRects.size()*trackHeight);
    trackRects.push_back(track);
    scene->addItem(track);

}

void TimelineWidget::mousePressEvent(QMouseEvent *event)
{
   QPoint widgetPoint = event->pos();
   QPointF scenePoint = view->mapToScene(view->mapFromParent(widgetPoint));
   //search items in scene
   for(const QGraphicsItem* item : scene->items()){
       if(item->contains(scenePoint)){
       }
       else{
           QPointF last = playhead->pos();
           playhead->updatePos(scenePoint);
           m_playheadPressed = true;
           if(last != playhead->pos())
           underPlayhead();

       }
   }

}

void TimelineWidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    m_playheadPressed = false;

}

void TimelineWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_playheadPressed){
        QPoint widgetPoint = event->pos();
        QPointF scenePoint = view->mapToScene(view->mapFromParent(widgetPoint));
        playhead->updatePos(scenePoint);
        underPlayhead();
    }
}

void TimelineWidget::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    //25pixles in 30frams
    //1 frame = 30/30 =
    playhead->setX(playhead->x()+ (float)((float)30/(float)30));
    qDebug() << playhead->pos().x();
    underPlayhead();
}

TimelineWidget::~TimelineWidget()
{
    delete ui;
}






