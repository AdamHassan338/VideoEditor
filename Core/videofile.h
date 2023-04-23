#ifndef VIDEOFILE_H
#define VIDEOFILE_H

#include<iostream>
#include "Core/videoreader.h"
#include <QObject>
#include <QThread>



class VideoFile : public QObject
{
        Q_OBJECT
public:
    //explicit VideoFile();
    explicit VideoFile(std::string location,QObject* parent = nullptr);

    ~VideoFile() = default;

    const std::string &name() const;
    int frames() const;
    double duration() const;
    double framerate() const;

    uint8_t* seekFrame(uint frameNumber);

    videoreader &decoder();

public slots:
    void backToMain();

private:
    std::string m_name;
    std::string m_location;
    std::string m_extention;

    int m_frames;
    double m_duration;
    double m_framerate;
    int m_width, m_height;

    videoreader m_decoder;
    QThread m_decodeThread;


};

#endif // VIDEOFILE_H
