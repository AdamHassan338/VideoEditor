extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
#include <libswscale/swscale.h>
#include <libavutil/avutil.h>
#include <libavutil/hwcontext.h>
#include <libavutil/opt.h>
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

    bool firstFrame = false;
    int m_timestamp;
    int64_t m_lastpts = 0;



public:
    struct videoReaderState {
        //video
        int width;
        int height;
        AVRational timeBase;
        uint8_t* imageBuffer;
        int64_t pts;

        //uint8_t frameBuffer;


        //ffmpeg
        AVFormatContext* formatContext;
        AVCodecContext* codecContext;
        int index;
        AVFrame* frame;
        AVFrame* Hwframe;
        AVPacket* packet;
        SwsContext* scalerContext;
        AVHWDeviceType hw_device_type = AV_HWDEVICE_TYPE_DXVA2;
        AVBufferRef* hw_device_ctx = nullptr;



    };



    explicit videoreader(QObject* parent = 0);
    ~videoreader();
    void run();
    bool stop();

    void videoReaderOpen(const char* filename);
    void videoReaderReadFrame(uint8_t* frameBuffer, int64_t* pts);
    void videoReaderSeekFrame(int64_t timestamp, int option);
    void videoReaderClose();

    void initializeScaler();

    void doSetup(QThread &cThread, int64_t pts);
    void setFrame(uint frameNumber);

    videoReaderState* getState() const;

signals:
    uint8_t newFrame(uint8_t* frameBuffer, int width, int height);

public slots:
    void doWork();
    //void seekFrame(uint frameNumber);

private:
    videoReaderState* state = new videoReaderState();
};


#endif // VIDEOREADER_H
