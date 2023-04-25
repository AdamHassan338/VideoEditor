#include "videowriter.h"
#include <QDebug>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
#include <libswscale/swscale.h>
}
videoWriter::videoWriter(QObject *parent)
    : QObject{parent}
{

    codec =  (AVCodec *)avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!codec) {
        fprintf(stderr, "codec not found\n");

    }
}

void videoWriter::setup(const char* filename)
{
    int ret;
    m_filename = filename;
    codecContext = avcodec_alloc_context3(codec);
    frame = av_frame_alloc();


    codecContext->bit_rate = 350000;
    codecContext->width = 500;
    codecContext->height = 500;

    frameBuffer = new uint8_t[codecContext->width * codecContext->height * 3];

    codecContext->time_base = (AVRational){1,30};
    codecContext->framerate = (AVRational){30,1};
    codecContext->gop_size = 10;
    codecContext->max_b_frames = 1;
    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;


    frame->format = codecContext->pix_fmt;
    frame->width  = codecContext->width;
    frame->height = codecContext->height;
    if(av_frame_get_buffer(frame,0) < 0){
        qDebug() << "could not make frame buffer";
    }

    frame->linesize[0] = frame->width;//y
    frame->linesize[1] = frame->width/2;//u
    frame->linesize[2] = frame->width/2;//v



    rgbaFrame->format = AV_PIX_FMT_RGB0;
    rgbaFrame->width  = codecContext->width;
    rgbaFrame->height = codecContext->height;
    if(av_frame_get_buffer(rgbaFrame,0) < 0){
        qDebug() << "could not make frame buffer";
    }

    rgbaFrame->linesize[0] = frame->width;//r
    rgbaFrame->linesize[1] = frame->width;//g
    rgbaFrame->linesize[2] = frame->width;//b
    rgbaFrame->linesize[2] = frame->width;//a



    if(avcodec_open2(codecContext,codec,NULL)<0){
        qDebug() << "could not open codec\n";

    }

    ret = av_frame_get_buffer(frame, 0);
    if (ret < 0) {
         qDebug() << "Could not allocate the video frame data\n";

    }

    packet = av_packet_alloc();
    if(!packet){
         qDebug() << "Could not allocate packet";
    }

    file = fopen(filename, "wb");

    if(!file){
        qDebug() << "could nto open " << filename << "\n";

    }

    size = codecContext->width * codecContext->height;



}


void videoWriter::initializeScaler()
{

    scalerContext = sws_getContext(codecContext->width, codecContext->height, AV_PIX_FMT_RGB0,
                                    codecContext->width, codecContext->height, AV_PIX_FMT_YUV420P,
                                    SWS_BILINEAR, NULL, NULL, NULL);

}

void videoWriter::encodeFrame(uint8_t *image)
{
    fflush(stdout);
    int ret;
    ret = av_frame_make_writable(frame);
    rgba2frame(image,rgbaFrame,rgbaFrame->width,rgbaFrame->height);
    sws_scale(scalerContext,frame->data,frame->linesize,0,frame->height,frame->data,frame->linesize);

    if (frame)
            printf("Send frame %3" PRId64 "\n", frame->pts);

        ret = avcodec_send_frame(codecContext, frame);
        if (ret < 0) {
            fprintf(stderr, "Error sending a frame for encoding\n");
            exit(1);
        }

        while (ret >= 0) {
            ret = avcodec_receive_packet(codecContext, packet);
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                return;
            else if (ret < 0) {
                fprintf(stderr, "Error during encoding\n");
                exit(1);
            }

            printf("Write packet %3" PRId64" (size=%5d)\n", packet->pts, packet->size);
            fwrite(packet->data, 1, packet->size, file);
            av_packet_unref(packet);
        }

}

void videoWriter::endFile()
{

    //encodeFrame(NULL);

    fclose(file);

        avcodec_free_context(&codecContext);
        av_frame_free(&frame);
        av_packet_free(&packet);

}

void videoWriter::rgba2frame(uint8_t *image, AVFrame *inframe, int width, int height)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            inframe->data[0][y * width*4 + x * 4] = image[y * width*4 + x * 4];
            inframe->data[1][y * width*4 + x * 4 + 1] = image[y * width*4 + x * 4 + 1];
            inframe->data[2][y * width*4 + x * 4 + 2 ] = image[y * width*4 + x * 4 + 2];
            inframe->data[0][y * width*4 + x * 4 + 3] = 1;
        }
    }
}
