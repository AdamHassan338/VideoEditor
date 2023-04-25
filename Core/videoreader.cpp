#include "videoreader.h"
#include <QDebug>
#include <QThread>

const char* videoreader::av_make_error(int errnum) {
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
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

void videoreader::videoReaderOpen(const char* filename){

    avcodec_configuration();



    state->formatContext = avformat_alloc_context();// open the container
    if(!state->formatContext){
        qDebug("could not create AVFormat context\n");
        return;
    }
    if(avformat_open_input(&state->formatContext, filename,NULL, NULL) != 0){
      qDebug("could not open video file\n");
      //avformat_free_context(context);
      return;
    }


    //find the first video stream
    state->index = 0;
    bool found = false;
    AVCodecParameters* params;
    AVCodec* codec;
    for(uint32_t i=0; i<state->formatContext->nb_streams; i++){
        auto stream = state->formatContext->streams[i];
        params = state->formatContext->streams[i]->codecpar;
        codec = const_cast<AVCodec*>(avcodec_find_decoder(params->codec_id));

        if(!codec){

            continue;
        }

        if(params->codec_type == AVMEDIA_TYPE_VIDEO){
            found = true;
            state->index = i;
            state->width = params->width;
            state->height = params->height;
            state->timeBase = state->formatContext->streams[i]->time_base;
            state->imageBuffer = new uint8_t[state->width * state->height * 4];

            break;
        }



        if(found){

            continue;
        }

        if(state->index == -1){
            qDebug("could not find video stream in file\n");
            return;
        }
    }


    state->codecContext = avcodec_alloc_context3(codec);
    if(!state->codecContext){
        qDebug("could not create codec context\n");
        return;
    }

    //set the hardware deivce
    //state->codecContext->hw_device_ctx = av_buffer_ref(state->hw_device_ctx);





    if(avcodec_parameters_to_context(state->codecContext,params)<0){
        qDebug("could not intialize codec context\n");
        return;
    }

    if(avcodec_open2(state->codecContext,codec,NULL)<0){
        qDebug("could not open codec\n");
        return;
    }

    //alocate a frame and packet

    state->packet = av_packet_alloc();
    state->frame = av_frame_alloc();

    //Hwframe = av_frame_alloc();
    if(!state->frame){
        qDebug("err could not allocate frame\n");
        return;
    }

    //if(!state->Hwframe){
        //qDebug("error could not allocate Hw frame\n");
        //return;
    //}

    if(!state->packet){
        qDebug("error could not allocate packet\n");
        return;
    }




    return;



}
void videoreader::videoReaderReadFrame(uint8_t* frameBuffer, int64_t* pts){

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
            return;
        }

        responce = avcodec_receive_frame(state->codecContext,state->frame);

        if(responce == AVERROR(EAGAIN) || responce == AVERROR(EOF)){

            av_packet_unref(state->packet);
            continue;
        }

        else if (state->frame->best_effort_timestamp == m_timestamp) {
            found = true;

            //av_hwframe_transfer_data(state->frame, state->Hwframe, 0);

            av_packet_unref(state->packet);
            //state->frame = state->Hwframe;
            break;
        }

        else if(responce < 0){

            qDebug("Failed to decode packet: %s \n", av_make_error(responce));
            return;

        }

        //av_packet_unref(state->packet);
        //break;

    }


    *pts = state->frame->pts;



    if(!state->scalerContext){

        initializeScaler();

    }




    if (!state->scalerContext) {
        qDebug("Couldn't initialize sw scaler\n");
        return;
    }


    //convert YUV data to RGBA


    uint8_t* output[4] = {frameBuffer,NULL,NULL,NULL};
    int outputLinesize [4] = {state->frame->width * 4,0,0,0};

    sws_scale(state->scalerContext,state->frame->data,state->frame->linesize,0,state->frame->height,output,outputLinesize);


    double pts_time = (double)state->frame->pts * (double)state->timeBase.num  / (double)state->timeBase.den;
    //qDebug() << "PTS: " << state->pts << "\n";
    // second to show = pts / timbase
    qDebug("frameTime:%fs\n", pts_time);




    emit newFrame(frameBuffer,state->width,state->height);





}

//int64_t seekPos = ((theFrameIndex/self.frameRate) * 1000000);

void videoreader::videoReaderSeekFrame(int64_t timestamp, int option)
{
    Q_UNUSED(option);
    m_timestamp = timestamp;

    av_seek_frame(state->formatContext,state->index,timestamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    avcodec_flush_buffers(state->codecContext);

    //state->pts = timestamp;
    qDebug() <<"timestamp: "<< timestamp << "\n";

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

    state->scalerContext = sws_getContext(state->width, state->height, static_cast<AVPixelFormat>(state->frame->format),
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


void videoreader::setFrame(uint frameNumber)
{
    AVStream* stream = state->formatContext->streams[state->index];
    double framerate = stream->avg_frame_rate.num / stream->avg_frame_rate.den;
    state->pts = (int64_t)(frameNumber * (double)state->timeBase.den / ((double)framerate * state->timeBase.num));


}


