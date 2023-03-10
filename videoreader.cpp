#include "videoreader.h"
#include <QDebug>
#include <QThread>
/*
CHANGE INDEX TO VIDOE STREAM INDEX

*/
const char* videoreader::av_make_error(int errnum) {
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

static AVPixelFormat correct_for_deprecated_pixel_format(AVPixelFormat pix_fmt) {
    // Fix swscaler deprecated pixel format warning
    // (YUVJ has been deprecated, change pixel format to regular YUV)
    switch (pix_fmt) {
        case AV_PIX_FMT_YUVJ420P: return AV_PIX_FMT_YUV420P;
        case AV_PIX_FMT_YUVJ422P: return AV_PIX_FMT_YUV422P;
        case AV_PIX_FMT_YUVJ444P: return AV_PIX_FMT_YUV444P;
        case AV_PIX_FMT_YUVJ440P: return AV_PIX_FMT_YUV440P;
        default:                  return pix_fmt;
    }
}

videoreader::videoreader(QObject* parent) : QObject(0){

}

bool videoreader::videoReaderOpen(videoReaderState* state, const char* filename){

    avcodec_configuration();
    AVFormatContext*& formatContext = state->formatContext;
    AVCodecContext*& codecContext = state->codecContext;
    int& index = state->index;
    AVFrame*& frame = state->frame;
    AVPacket*& packet = state->packet;
    SwsContext*& scalerContext = state->scalerContext;
    int& width = state->width;
    int& height = state->height;


    formatContext = avformat_alloc_context();// open the container
    if(!formatContext){
        qDebug("could not create AVFormat context\n");
        return false;
    }
    if(avformat_open_input(&formatContext, filename,NULL, NULL) != 0){
      qDebug("could not open video file\n");
      //avformat_free_context(context);
      return false;
    }
    //find the first video stream
    index = -1;
    bool found = false;
    AVCodecParameters* params;
    AVCodec* codec;
    for(uint32_t i=0; i<formatContext->nb_streams; i++){
        auto stream = formatContext->streams[i];
        params = formatContext->streams[i]->codecpar;
        codec = const_cast<AVCodec*>(avcodec_find_decoder(params->codec_id));

        if(!codec){
            index=-2;
            continue;
        }

        if(params->codec_type == AVMEDIA_TYPE_VIDEO){
            found = true;
            index = i;
            width = params->width;
            height = params->height;
            state->timeBase = formatContext->streams[i]->time_base;
            //state->timeBase.den = formatContext->streams[i]->r_frame_rate.den * 1000;
            break;
        }



        if(found){

            continue;
        }

        if(index == -1){
            qDebug("could not find video stream in file\n");
            return false;
        }
    }

    //setup codec context for decoder
    codecContext = avcodec_alloc_context3(codec);
    if(!codecContext){
        qDebug("could not create AVCodecContext\n");
        return false;
    }



    if(avcodec_parameters_to_context(codecContext,params)<0){
        qDebug("could not intialize AVCodecContext\n");
        return false;
    }

    if(avcodec_open2(codecContext,codec,NULL)<0){
        qDebug("could not open codec\n");
        return false;
    }

    //alocate a frame and packet

    packet = av_packet_alloc();
    frame = av_frame_alloc();
    if(!frame){
        qDebug("err could not allocate AVFrame\n");
        return false;
    }

    if(!packet){
        qDebug("err could not allocate AVPacket\n");
        return false;
    }




    return true;



}
bool videoreader::videoReaderReadFrame(videoReaderState* state, uint8_t* frameBuffer, int64_t* pts){
/*
    vrState.formatContext;
    vrState.codecContext;
    vrState.index;
    vrState.frame;
    vrState.packet;
    vrState.scalerContext;
    vrState.width;
    vrState.height;*/

    //decode 1 frame

   // qDebug("in the thing");

    int responce;
    while(av_read_frame(vrState.formatContext,vrState.packet) >= 0){
        if(vrState.packet->stream_index != vrState.index){
            av_packet_unref(vrState.packet);
            continue;
        }
       responce = avcodec_send_packet(vrState.codecContext,vrState.packet);
        if(responce<0){
            qDebug("Failed to decode packet\n");
            return false;
        }
        responce = avcodec_receive_frame(vrState.codecContext,vrState.frame);
        if(responce == AVERROR(EAGAIN) || responce == AVERROR(EOF)){
            av_packet_unref(vrState.packet);
            continue;
        }else if(responce < 0){

            qDebug("Failed to decode packet: %s \n", av_make_error(responce));
            return false;
        }
        av_packet_unref(vrState.packet);
        break;

    }

    *pts = vrState.frame->pts;

    qDebug(
        "Frame %c (%d) pts %d dts %d key_frame %d [coded_picture_number %d, display_picture_number %d]\n",
        av_get_picture_type_char(vrState.frame->pict_type),
        vrState.codecContext->frame_number,
        vrState.frame->pts,
        vrState.frame->pkt_dts,
        vrState.frame->key_frame,
        vrState.frame->coded_picture_number,
        vrState.frame->display_picture_number
    );


    // Set up sws scaler
    // need to find a way to do this only once

       // auto source_pix_fmt = correct_for_deprecated_pixel_format(codecContext->pix_fmt);
        vrState.scalerContext = sws_getContext(vrState.width, vrState.height, vrState.codecContext->pix_fmt,
                                        vrState.width, vrState.height, AV_PIX_FMT_RGB0,
                                        SWS_BILINEAR, NULL, NULL, NULL);

    if (!vrState.scalerContext) {
        qDebug("Couldn't initialize sw scaler\n");
        return false;
    }


    //convert YUV data to RGBA
    //uint8_t* rgbData = new uint8_t[frame->width * frame->height * 4];

    uint8_t* dest[4] = {frameBuffer,NULL,NULL,NULL};
    int destLinesize [4] = {vrState.frame->width * 4,0,0,0};

    sws_scale(vrState.scalerContext,vrState.frame->data,vrState.frame->linesize,0,vrState.frame->height,dest,destLinesize);

    //if(*pts == 58500 || *pts == 81000 || *pts == 129000)
    //
    if(!firstFrame){
        time.start();
        firstFrame = true;
    }


    //qDebug("time:%lldms\n", time.elapsed() );
    double pts_time = vrState.frame->pts * (double)vrState.timeBase.num  / (double)vrState.timeBase.den;
    // second to show = pts / timbase
    qDebug("frameTime:%fs\n", pts_time);


    //double sleepTime = pts_time*1000 - (time.elapsed());
    //qDebug("sleep for :%fms",sleepTime);
    //while( time.elapsed() < pts_time*1000){
        //qDebug("waiting");
    //}


    qDebug("emmit frame now at time:%lldms\n", time.elapsed());
    emit videoreader::newFrame(frameBuffer);
    QThread::msleep(10);
    double sleepTime = pts_time*1000 - (time.elapsed());
    if(sleepTime > 0)
        qDebug("sleep for :%fms",sleepTime);
        //QThread::msleep(sleepTime);

    return true;



}

//int64_t seekPos = ((theFrameIndex/self.frameRate) * 1000000);

bool videoreader::videoReaderSeekFrame(videoReaderState *state, int64_t timestamp, int rel)
{
    //avformat_seek_file(vrState.formatContext,vrState.index,timestamp,timestamp,timestamp,AVSEEK_FLAG_BACKWARD);

    av_seek_frame(vrState.formatContext,vrState.index,timestamp,rel);
    avcodec_flush_buffers(vrState.codecContext);

/*
    int responce;
    while(av_read_frame(vrState.formatContext,vrState.packet) >= 0){
        if(vrState.packet->stream_index != vrState.index){
            av_packet_unref(vrState.packet);
            continue;
        }
       responce = avcodec_send_packet(vrState.codecContext,vrState.packet);
        if(responce<0){
            qDebug("Failed to decode packet\n");
            return false;
        }
        responce = avcodec_receive_frame(vrState.codecContext,vrState.frame);
        if(responce == AVERROR(EAGAIN) || responce == AVERROR(EOF)){
            av_packet_unref(vrState.packet);
            continue;
        }else if(responce < 0){

            qDebug("Failed to decode packet: %s \n", av_make_error(responce));
            return false;
        }
        av_packet_unref(vrState.packet);
        break;

    }

*/
    return true;
}
void videoreader::videoReaderClose(videoReaderState* state){

    sws_freeContext(state->scalerContext);
    avformat_close_input(&state->formatContext);
    avformat_free_context(state->formatContext);
    avcodec_free_context(&state->codecContext);
    av_packet_free(&state->packet);
    av_frame_free(&state->frame);

}

void videoreader::doWork(){
        qDebug("do work");
        //time.start();
        videoreader::videoReaderSeekFrame(&vrState, 30 * ((double)vrState.timeBase.den / (double )vrState.timeBase.num  ),0 );
        videoreader::videoReaderReadFrame(&vrState, vrState.imageBuffer,&vrState.pts);
        QThread::msleep(2000);
        videoreader::videoReaderSeekFrame(&vrState,(double)(9.5 * ((double)vrState.timeBase.den / (double )vrState.timeBase.num )),AVSEEK_FLAG_ANY);
        videoreader::videoReaderReadFrame(&vrState, vrState.imageBuffer,&vrState.pts);
        //while(true){

           //videoreader::videoReaderReadFrame(&vrState, vrState.imageBuffer,&vrState.pts);
            //QThread::msleep(10);
       // }


}

void videoreader::doSetup(QThread &cThread, int64_t pts){
    connect(&cThread,SIGNAL(started()),this,SLOT(doWork()));
    //connect(&cThread,SIGNAL(started()),this,SLOT(doWork()));
    vrState.pts = pts;
}

void videoreader::setState(videoReaderState state){
    state.imageBuffer =  new uint8_t[state.width * state.height * 4];
    vrState = state;
}


//dest[4] = {frameBuffer,NULL,NULL,NULL};

//state->width = frame->width;
//state->height = frame->height;
//frameBuffer = rgbData;
