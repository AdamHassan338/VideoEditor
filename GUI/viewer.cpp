#include "viewer.h"

Viewer::Viewer(QWidget *parent)
    : QLabel(parent)
{

}


void Viewer::init(int width, int height)
{
    m_width = width; m_height = height;
    QImage img(m_width,m_height,QImage::Format_RGB888);
    img.fill(Qt::black);
    pix = QPixmap::fromImage(img);
    setPixmap(pix);
}

void Viewer::updateViewer(uint8_t *frameData, int width, int height)
{
    //first create image of frameData size
    pix = QPixmap(width,height);
    QImage imageBuffer = QImage(QPixmap(width,height).toImage());
    QRgb value;
    for(int x = 0; x<pix.width(); ++x){
        for(int y = 0; y<pix.height(); ++y){
            //imageBuffer.setPixel(x,y,qRgb(255,0,0));
            value = qRgba(frameData[y * pix.width() * 4 + x * 4],frameData[y * pix.width() * 4 + x *4 + 1],frameData[y * pix.width() * 4 + x *4 + 2],0);
            imageBuffer.setPixel(x,y,value);
        }
    }

    //endloop:
    //pix=QPixmap::fromImage(imageBuffer);

    QImage scaled = imageBuffer.scaled(m_width,m_height, Qt::IgnoreAspectRatio);

    pix = QPixmap::fromImage(scaled);





    //pix = pix.scaled(this->width(),this->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
    //pix=QPixmap::fromImage(QImage(data, frameWidth, frameHeight, QImage::Format_RGB888));
    setPixmap(QPixmap::fromImage(scaled));

}

void Viewer::updateViewerBlank()
{

    QImage img(m_width,m_height,QImage::Format_RGB888);
    img.fill(Qt::black);
    pix = QPixmap::fromImage(img);
    setPixmap(pix);
}

