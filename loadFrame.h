extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <inttypes.h>
#include <libswscale/swscale.h>
}
#ifndef LOADFRAME_H
#define LOADFRAME_H

#endif // LOADFRAME_H

bool loadFrame(const char* filename,int* widthOut,int* heightOut,unsigned char** dataOut);
