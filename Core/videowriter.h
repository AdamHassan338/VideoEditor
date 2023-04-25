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
#include <string>

class videoWriter : public QObject
{
    Q_OBJECT
public:
    explicit videoWriter(QObject *parent = nullptr);

    void setup(const char* filename);
    void initializeScaler();
    void encodeFrame(uint8_t* image);

    void endFile();


private:
    AVCodec* codec;
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;

    int index;
    FILE* file;
    AVFrame* frame;
    AVFrame* rgbaFrame;
    AVPacket* packet;
    SwsContext* scalerContext;
    std::string m_filename;



    void rgba2frame(uint8_t* image,AVFrame* frame,int width,int height);

signals:

};

#endif // VIDEOWRITER_H
