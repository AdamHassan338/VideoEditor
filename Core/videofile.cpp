#include "videofile.h"
#include <QApplication>


VideoFile::VideoFile(std::string location, QObject* parent) : QObject{parent}, m_location(location)
{
    bool extentionRead = false;
    for(uint i=m_location.size()-1; i>=0;i--){
        if(m_location.at(i)=='.' && !extentionRead){
            extentionRead=true;
            std::reverse(m_extention.begin(),m_extention.end());
            i--;
        }else if(m_location.at(i)=='\\' || m_location.at(i)=='/'){
            std::reverse(m_name.begin(),m_name.end());
            break;
        }
        if(!extentionRead){
            m_extention.push_back(m_location.at(i));
        }else{
            m_name.push_back(m_location.at(i));
        }
    }
    m_decoder.videoReaderOpen(location.c_str());


    int index = m_decoder.getState()->index;
    AVStream* stream =m_decoder.getState()->formatContext->streams[index];
    m_framerate = stream->avg_frame_rate.num / stream->avg_frame_rate.den;
    m_frames = stream->nb_frames;
    m_width = m_decoder.getState()->width;
    m_height = m_decoder.getState()->height;
    m_duration = (double)stream->duration * (double)((double)m_decoder.getState()->timeBase.num/(double)m_decoder.getState()->timeBase.den);
    int a = 2;
    connect(&m_decodeThread,SIGNAL(started()),&m_decoder,SLOT(doWork()));
    connect(&m_decoder,&videoreader::newFrame,this,&VideoFile::backToMain);
    m_decoder.moveToThread(&m_decodeThread);



}

const std::string &VideoFile::name() const
{
    return m_name;
}

int VideoFile::frames() const
{
    return m_frames;
}

double VideoFile::duration() const
{
    return m_duration;
}

double VideoFile::framerate() const
{
    return m_framerate;
}

uint8_t* VideoFile::seekFrame(uint frameNumber)
{

    m_decoder.setFrame(frameNumber);


    m_decodeThread.start();
    m_decodeThread.quit();
    m_decodeThread.wait();

}

videoreader &VideoFile::decoder()
{
    return m_decoder;
}

void VideoFile::backToMain()
{
    //m_decoder.moveToThread(QApplication::instance()->thread());


    //m_decodeThread.finished();
}
