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

    void setup(const char* filename);
    void initializeScaler();
    void encodeFrame(uint8_t* image);

    void endFile();

private:
    AVCodec* codec;
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    uint8_t* frameBuffer;
    int index;
    FILE* file;
    int i, out_size, size, x, y, outbuf_size;
    uint8_t *outbuf, *picture_buf;
    AVFrame* frame;
    AVFrame* rgbaFrame;
    AVPacket* packet;
    SwsContext* scalerContext;
    const char* m_filename;

    uint8_t endcode[4] = { 0, 0, 1, 0xb7 };

    void rgba2frame(uint8_t* image,AVFrame* frame,int width,int height);

signals:

};

#endif // VIDEOWRITER_H
