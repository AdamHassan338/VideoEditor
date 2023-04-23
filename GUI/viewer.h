#ifndef VIEWER_H
#define VIEWER_H

#include <QWidget>
#include <QLabel>

class Viewer : public QLabel
{
    Q_OBJECT
public:
    explicit Viewer(QWidget *parent = nullptr);
    void init(int width, int height);

public slots:
    void updateViewer(uint8_t* frameData,int width,int height);
    void updateViewerBlank();

private:
    QPixmap pix;
    int m_width,m_height;

signals:

};

#endif // VIEWER_H
