#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPixmap>
#include "Core/videoreader.h"
#include <QThread>
#include <qtimer.h>

//8bit RGB image EACH channel has 8bits as range is 0-255, 8+8+8 so a total of 24bits
//1 byte = 8 bits

int frameWidth = 960,frameHeight = 540;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label_image->init(960,540);

    QMenu *fileMenu = ui->menubar->addMenu("File");
    QAction *exportAction = new QAction("Export", this);
    fileMenu->addAction(exportAction);


    m_controller = new TimelineController(this,ui->timelineWidget);

    connect(m_controller,&TimelineController::updateSignal,ui->label_image,&Viewer::updateViewer);
    connect(m_controller,&TimelineController::updateBlankSignal,ui->label_image,&Viewer::updateViewerBlank);
    connect(exportAction,&QAction::triggered,m_controller,&TimelineController::startRender);

}


MainWindow::~MainWindow()
{

    delete ui;

}

