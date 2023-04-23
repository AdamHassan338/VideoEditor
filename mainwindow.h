#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
//#include "Core\videoreader.h"
#include "timelinecontroller.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void updateVideo();
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QPixmap pix;
    TimelineController* m_controller = nullptr;

    //videoreader vr;
    //QThread decoderThread;
    //QThread testThread;

private slots:
    void updateRender(uint8_t* frameData);

};
#endif // MAINWINDOW_H
