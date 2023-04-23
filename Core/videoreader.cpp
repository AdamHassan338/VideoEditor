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

static enum AVPixelFormat get_hw_format(AVCodecContext* ctx, const enum AVPixelFormat* pix_fmts) {
    const enum AVPixelFormat* p;
    for (p = pix_fmts; *p != AV_PIX_FMT_NONE; p++) {
        if (*p == AV_PIX_FMT_VAAPI) {
            return *p;
        }
    }
    return AV_PIX_FMT_NONE;
}

videoreader::videoreader(QObject* parent) : QObject(0){
    if (av_hwdevice_ctx_create(&state->hw_device_ctx, state->hw_device_type, nullptr, nullptr, 0) < 0) {
        qDebug("error creating hw");
    }

}

videoreader::~videoreader()
{
    videoReaderClose();
    delete state;
}

bool videoreader::videoReaderOpen(const char* filename){

    avcodec_configuration();
    AVFormatContext*& formatContext = state->formatContext;
    AVCodecContext*& codecContext = state->codecContext;
    int& index = state->index;
    AVFrame*& frame = state->frame;
    AVFrame*& Hwframe = state->Hwframe;
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
            state->imageBuffer = new uint8_t[width * height * 4];
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
    //set the hardware deivce
    state->codecContext->hw_device_ctx = av_buffer_ref(state->hw_device_ctx);





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

    Hwframe = av_frame_alloc();
    if(!frame){
        qDebug("err could not allocate AVFrame\n");
        return false;
    }

    if(!Hwframe){
        qDebug("err could not allocate Hw AVFrame\n");
        return false;
    }

    if(!packet){
        qDebug("err could not allocate AVPacket\n");
        return false;
    }




    return true;



}
bool videoreader::videoReaderReadFrame(uint8_t* frameBuffer, int64_t* pts){

    bool found = false;
    int responce;
    while(!found &&av_read_frame(state->formatContext,state->packet) >= 0){
        if(state->packet->stream_index != state->index){
            av_packet_unref(state->packet);
            continue;
        }

       responce = avcodec_send_packet(state->codecContext,state->packet);

        if(responce<0){
            qDebug("Failed to decode packet\n");
            return false;
        }

        responce = avcodec_receive_frame(state->codecContext,state->Hwframe);

        if(responce == AVERROR(EAGAIN) || responce == AVERROR(EOF)){

            av_packet_unref(state->packet);
            continue;
        }

        else if (state->Hwframe->best_effort_timestamp == m_timestamp) {
            found = true;

            av_hwframe_transfer_data(state->frame, state->Hwframe, 0);

            av_packet_unref(state->packet);
            //state->frame = state->Hwframe;
            break;
        }

        else if(responce < 0){

            qDebug("Failed to decode packet: %s \n", av_make_error(responce));
            return false;

        }

        //av_packet_unref(state->packet);
        //break;

    }


    *pts = state->frame->pts;

    qDebug(
        "Frame %c (%d) pts %d dts %d key_frame %d [coded_picture_number %d, display_picture_number %d]\n",
        av_get_picture_type_char(state->frame->pict_type),
        state->codecContext->frame_number,
        state->frame->pts,
        state->frame->pkt_dts,
        state->frame->key_frame,
        state->frame->coded_picture_number,
        state->frame->display_picture_number
    );


    // Set up sws scaler
    // ONLY DO THIS ONCE

    if(!state->scalerContext){
        // auto source_pix_fmt = correct_for_deprecated_pixel_format(codecContext->pix_fmt);
        initializeScaler();

    }




    if (!state->scalerContext) {
        qDebug("Couldn't initialize sw scaler\n");
        return false;
    }


    //convert YUV data to RGBA
    //uint8_t* rgbData = new uint8_t[frame->width * frame->height * 4];

    uint8_t* dest[4] = {frameBuffer,NULL,NULL,NULL};
    int destLinesize [4] = {state->frame->width * 4,0,0,0};

    sws_scale(state->scalerContext,state->frame->data,state->frame->linesize,0,state->frame->height,dest,destLinesize);


    double pts_time = (double)state->frame->pts * (double)state->timeBase.num  / (double)state->timeBase.den;
    //qDebug() << "PTS: " << state->pts << "\n";
    // second to show = pts / timbase
    qDebug("frameTime:%fs\n", pts_time);



    qDebug("emmit frame now at time:%lldms\n", time.elapsed());
    emit videoreader::newFrame(frameBuffer,state->width,state->height);

    return true;



}

//int64_t seekPos = ((theFrameIndex/self.frameRate) * 1000000);

bool videoreader::videoReaderSeekFrame(int64_t timestamp, int rel)
{
    m_timestamp = timestamp;

    int ret = av_seek_frame(state->formatContext,state->index,timestamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    avcodec_flush_buffers(state->codecContext);

    //state->pts = timestamp;
    qDebug() <<"timestamp: "<< timestamp << "\n";
    return true;
}
void videoreader::videoReaderClose(){

    sws_freeContext(state->scalerContext);
    avformat_close_input(&state->formatContext);
    avformat_free_context(state->formatContext);
    avcodec_free_context(&state->codecContext);
    av_packet_free(&state->packet);
    av_frame_free(&state->frame);

}

void videoreader::initializeScaler()
{

    state->scalerContext = sws_getContext(state->width, state->height, static_cast<AVPixelFormat>(state->Hwframe->format),
                                    state->width, state->height, AV_PIX_FMT_RGB0,
                                    SWS_BILINEAR, NULL, NULL, NULL);

}

void videoreader::doWork(){
        qDebug("do work");


        videoreader::videoReaderSeekFrame(state->pts, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
        videoreader::videoReaderReadFrame(state->imageBuffer,&state->pts);



}

videoreader::videoReaderState *videoreader::getState() const
{
    return state;
}

void videoreader::doSetup(QThread &cThread, int64_t pts){
    connect(&cThread,SIGNAL(started()),this,SLOT(doWork()));
    //connect(&cThread,SIGNAL(started()),this,SLOT(doWork()));
    state->pts = pts;
}

void videoreader::setState(videoReaderState instate){
    state->imageBuffer =  new uint8_t[instate.width * instate.height * 4];
    state = &instate;
}

void videoreader::setFrame(uint frameNumber)
{
    AVStream* stream = state->formatContext->streams[state->index];
    double framerate = stream->avg_frame_rate.num / stream->avg_frame_rate.den;
    state->pts = (int64_t)(frameNumber * (double)state->timeBase.den / ((double)framerate * state->timeBase.num));

    int a =2;
}





//dest[4] = {frameBuffer,NULL,NULL,NULL};

//state->width = frame->width;
//state->height = frame->height;
//frameBuffer = rgbData;
