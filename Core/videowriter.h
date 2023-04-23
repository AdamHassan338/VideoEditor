#ifndef VIDEOWRITER_H
#define VIDEOWRITER_H


extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
#include <libswscale/swscale.h>
}
#include <QObject>
#include <QThread>
#include <QObject>

class videoWriter : public QObject
{
    Q_OBJECT
public:
    explicit videoWriter(QObject *parent = nullptr);

private:
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    int index;
    AVFrame* frame;
    AVPacket* packet;
    SwsContext* scalerContext;

signals:

};

#endif // VIDEOWRITER_H
