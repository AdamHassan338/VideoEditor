#include <string>
#include "loadFrame.h"


// av_err2str returns a temporary array. This doesn't work in gcc.
// This function can be used as a replacement for av_err2str.
static const char* av_make_error(int errnum) {
    static char str[AV_ERROR_MAX_STRING_SIZE];
    memset(str, 0, sizeof(str));
    return av_make_error_string(str, AV_ERROR_MAX_STRING_SIZE, errnum);
}

bool loadFrame(const char* filename,int* widthOut,int* heightOut,unsigned char** dataOut){
    avcodec_configuration();
    AVFormatContext* context;
    AVCodecContext* codecContext;
    AVFrame* frame;
    AVPacket* packet;
    SwsContext* scalerContext;


    context = avformat_alloc_context();// open the container
    if(!context){
        printf("could not create AVFormat context\n");
        return false;
    }
    if(avformat_open_input(&context, filename,NULL, NULL) != 0){
      printf("could not open video file\n");
      //avformat_free_context(context);
      return false;
    }
    //find the first video stream
    int index = -1;
    bool found = false;
    AVCodecParameters* params;
    AVCodec* codec;
    for(uint32_t i=0; i<context->nb_streams; i++){
        //auto stream = context->streams[i];
        params = context->streams[i]->codecpar;
        codec = const_cast<AVCodec*>(avcodec_find_decoder(params->codec_id));

        if(!codec){
            index=-2;
            continue;
        }

        if(params->codec_type == AVMEDIA_TYPE_VIDEO){
            found = true;
            index = i;
            break;
        }



        if(found){

            continue;
        }

        if(index == -1){
            printf("could not find video stream in file\n");
            return false;
        }
    }

    //setup codec context for decoder
    codecContext = avcodec_alloc_context3(codec);
    if(!codecContext){
        printf("could not create AVCodecContext\n");
        return false;
    }



    if(avcodec_parameters_to_context(codecContext,params)<0){
        printf("could not intialize AVCodecContext\n");
        return false;
    }

    if(avcodec_open2(codecContext,codec,NULL)<0){
        printf("could not open codec\n");
        return false;
    }

    packet = av_packet_alloc();
    frame = av_frame_alloc();
    if(!frame){
        printf("err could not allocate AVFrame\n");
        return false;
    }

    if(!packet){
        printf("err could not allocate AVPacket\n");
        return false;
    }

    int responce;
    while(av_read_frame(context,packet) >= 0){
        if(packet->stream_index != index)
            continue;
       responce = avcodec_send_packet(codecContext,packet);
        if(responce<0){
            printf("Failed to decode packet\n");
            return false;
        }
        responce = avcodec_receive_frame(codecContext,frame);
        if(responce == AVERROR(EAGAIN) || responce == AVERROR(EOF)){
            continue;
        }else if(responce < 0){

            printf("Failed to decode packet: %s \n", av_make_error(responce));
            return false;
        }
        av_packet_unref(packet);
        break;

    }
    /*
    unsigned char* data = new unsigned char[frame->width * frame->height *3];
    for(int x =0; x<frame->width; x++){
        // x for column number
        for(int y =0; y<frame->height; y++){
            //y for row number
            data[y * frame->width * 3 + x * 3    ] = frame->data[0][y * frame->linesize[0] + x] ;
            data[y * frame->width * 3 + x * 3 + 1] = frame->data[0][y * frame->linesize[0] + x] ;
            data[y * frame->width * 3 + x * 3 + 2] = frame->data[0][y * frame->linesize[0] + x] ;
        }
    }

    *widthOut = frame->width;
    *heightOut = frame->height;
    *dataOut = data;*/

    scalerContext = sws_getContext(frame->width,frame->height,
                                           codecContext->pix_fmt,
                                           frame->width,frame->height,
                                           AV_PIX_FMT_RGB0,
                                           SWS_BICUBIC,NULL,NULL,NULL);
    if(!scalerContext){
        printf("could not initalise sw scaler\n");
        return false;
    }

    uint8_t* rgbData = new uint8_t[frame->width * frame->height * 4];

    uint8_t* dest[4] = {rgbData,NULL,NULL,NULL};
    int destLinesize [4] = {frame->width * 4,0,0,0};

    sws_scale(scalerContext,frame->data,frame->linesize,0,frame->height,dest,destLinesize);

    *widthOut = frame->width;
    *heightOut = frame->height;
    *dataOut = rgbData;
    sws_freeContext(scalerContext);
    avformat_close_input(&context);
    avformat_free_context(context);
    avcodec_free_context(&codecContext);
    av_packet_free(&packet);
    av_frame_free(&frame);


return true;
}
