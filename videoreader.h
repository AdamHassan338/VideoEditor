extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
#include <libswscale/swscale.h>
}
#include <QObject>
#include <QThread>
#include <QElapsedTimer>
#ifndef VIDEOREADER_H
#define VIDEOREADER_H

class videoreader : public QObject{

    Q_OBJECT
private:


    static const char* av_make_error(int errnum);
    QElapsedTimer time;
    bool firstFrame = false;



public:
    struct videoReaderState {
        //public things for other parts of program to read from
        int width;
        int height;
        AVRational timeBase;
        uint8_t* imageBuffer;
        int64_t pts;

        //uint8_t frameBuffer;


        //private internal
        AVFormatContext* formatContext;
        AVCodecContext* codecContext;
        int index;
        AVFrame* frame;
        AVPacket* packet;
        SwsContext* scalerContext;

    };



    explicit videoreader(QObject* parent = 0);
    void run();
    bool stop();

    static bool videoReaderOpen(videoReaderState* state, const char* filename);
    bool videoReaderReadFrame(videoReaderState* state, uint8_t* frameBuffer, int64_t* pts);
    bool videoReaderSeekFrame(videoReaderState* state, int64_t timestamp, int rel);
    static void videoReaderClose(videoReaderState* state);

    void doSetup(QThread &cThread, int64_t pts);
    void setState(videoReaderState state);

signals:
    uint8_t newFrame(uint8_t* frameBuffer);

public slots:
    void doWork();

private:
    videoReaderState vrState;
};
/*

struct videoReaderState {
    //public things for other parts of program to read from
    int width;
    int height;
    AVRational timeBase;

    //uint8_t frameBuffer;


    //private internal
    AVFormatContext* formatContext;
    AVCodecContext* codecContext;
    int index;
    AVFrame* frame;
    AVPacket* packet;
    SwsContext* scalerContext;

};


bool videoReaderOpen(videoReaderState* state, const char* filename);
bool videoReaderReadFrame(videoReaderState* state, uint8_t* frameBuffer, int64_t* pts);
void videoReaderClose(videoReaderState* state);

*/



#endif // VIDEOREADER_H
