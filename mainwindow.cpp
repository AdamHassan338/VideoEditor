#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
//#include "loadFrame.h"
#include "Core/videoreader.h"
#include <QThread>
#include <qtimer.h>

//8bit RGB image EACH channel has 8bits as range is 0-255, 8+8+8 so a total of 24bits
//c++ arrays using bytes, 1 byte = 8 bits

int frameWidth = 960,frameHeight = 540;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_image->init(960,540);
    m_controller = new TimelineController(this,ui->timelineWidget);
    //vr = videoreader();
    //connect(&vr,&videoreader::newFrame,this,&MainWindow::updateRender);
    connect(m_controller,&TimelineController::updateSignal,ui->label_image,&Viewer::updateViewer);
    connect(m_controller,&TimelineController::updateBlankSignal,ui->label_image,&Viewer::updateViewerBlank);



    unsigned char* data;

    uint8_t* frameData;
    //videoreader::videoReaderState vrState;
    int64_t pts;

    /*
    if(!videoreader::videoReaderOpen(&vrState,"C:/Users/Adam/Downloads/jDDJHxQTMYdNYnVf.mp4")){
        printf("could not open video file\n");
        //ui->label_image->setText("could not load video frame");
    }*/
    //frameWidth = vrState.width;
    //frameHeight = vrState.height;
    //pix = QPixmap(frameWidth,frameHeight);
    //frameData = new uint8_t[vrState.width * vrState.height * 4];//RGBA
    /*
    vr.setState(vrState);
    vr.doSetup(decoderThread, pts);
    vr.moveToThread(&decoderThread);
    decoderThread.start();
    //testThread.start();
    qDebug("not in the thing");
    */




}

void MainWindow::updateRender(uint8_t* frameData){
    //qDebug("we did it");

    pix = QPixmap(frameWidth,frameHeight);
    QImage imageBuffer = QImage(pix.toImage());
    QRgb value;
    for(int x = 0; x<pix.width(); ++x){
        for(int y = 0; y<pix.height(); ++y){
            //imageBuffer.setPixel(x,y,qRgb(255,0,0));
            value = qRgba(frameData[y * pix.width() * 4 + x * 4],frameData[y * pix.width() * 4 + x *4 + 1],frameData[y * pix.width() * 4 + x *4 + 2],0);
            imageBuffer.setPixel(x,y,value);
        }
    }
    endloop:
    pix=QPixmap::fromImage(imageBuffer);
    //pix = pix.scaled(this->width(),this->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    //pix=QPixmap::fromImage(QImage(data, frameWidth, frameHeight, QImage::Format_RGB888));
    //ui->label_image->setPixmap(pix);

}


MainWindow::~MainWindow()
{
    //decoderThread.quit();
    delete ui;

}



/*
    if(!loadFrame("C:/Users/Adam/Downloads/RyiH6uXhuod0bkFP.mp4",&frameWidth,&frameHeight, &data)){
        printf("could not load video frame\n");
        //ui->label_image->setText("could not load video frame");
    } */

   // ui->label_image->setFixedWidth(frameWidth);
   // ui->label_image->setFixedHeight(frameHeight);

/*
    if(!videoreader::videoReaderReadFrame(&vrState,frameData,&pts)){
        printf("could not load video frame\n");
    }
    QPixmap pix = QPixmap(frameWidth,frameHeight);
    QImage imageBuffer = QImage(pix.toImage());
    QRgb value;
    for(int x = 0; x<frameWidth; ++x){
        for(int y = 0; y<frameHeight; ++y){
            //imageBuffer.setPixel(x,y,qRgb(255,0,0));
            value = qRgba(frameData[y * frameWidth * 4 + x * 4],frameData[y * frameWidth * 4 + x *4 + 1],frameData[y * frameWidth * 4 + x *4 + 2],0);
            imageBuffer.setPixel(x,y,value);
        }
    }
    pix=QPixmap::fromImage(imageBuffer);
    //pix=QPixmap::fromImage(QImage(data, frameWidth, frameHeight, QImage::Format_RGB888));
    ui->label_image->setPixmap(pix);









    videoReaderClose(&vrState);
    /*


/*
    QPixmap pix("C:/Users/Adam/Documents/city/Individual project/Code/ImportTest/video/image.png");

    ui->label_image->setFixedWidth(pix.width());
    ui->label_image->setFixedHeight(pix.height());
    QImage temp(pix.toImage());
    QRgb value = qRgb(255,255,0);
       for (int y = 0; y < temp.height(); y++)
       {
        temp.setPixel(y,y,value);
       }
    pix=QPixmap::fromImage(temp);
    ui->label_image->setPixmap(pix);
*/

