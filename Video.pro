QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Core/videofile.cpp \
    Core/videoreader.cpp \
    Core/videowriter.cpp \
    GUI/clipitemhandler.cpp \
    GUI/cliprect.cpp \
    GUI/playhead.cpp \
    GUI/playheadhandler.cpp \
    GUI/timelinescene.cpp \
    GUI/timelineview.cpp \
    GUI/timelinewidget.cpp \
    GUI/trackrect.cpp \
    GUI/viewer.cpp \
    main.cpp \
    mainwindow.cpp \
    timelinecontroller.cpp

HEADERS += \
    Core/videofile.h \
    Core/videoreader.h \
    Core/videowriter.h \
    GUI/clipitemhandler.h \
    GUI/cliprect.h \
    GUI/common.h \
    GUI/playhead.h \
    GUI/playheadhandler.h \
    GUI/timelinescene.h \
    GUI/timelineview.h \
    GUI/timelinewidget.h \
    GUI/trackrect.h \
    GUI/viewer.h \
    mainwindow.h \
    timelinecontroller.h

FORMS += \
    GUI/timelinewidget.ui \
    mainwindow.ui

INCLUDEPATH += libs

LIBS += -L"lib" -lavcodec

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavcodec
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavcodec
else:unix: LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavcodec

INCLUDEPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
DEPENDPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavdevice
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavdevice
else:unix: LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavdevice

INCLUDEPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
DEPENDPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavfilter
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavfilter
else:unix: LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavfilter

INCLUDEPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
DEPENDPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavformat
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavformat
else:unix: LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavformat

INCLUDEPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
DEPENDPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavutil
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavutil
else:unix: LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lavutil

INCLUDEPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
DEPENDPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lswresample
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lswresample
else:unix: LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lswresample

INCLUDEPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
DEPENDPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lswscale
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lswscale
else:unix: LIBS += -L$$PWD/lib/ffmpeg-5.1.2-full_build-shared/lib/ -lswscale

INCLUDEPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
DEPENDPATH += $$PWD/lib/ffmpeg-5.1.2-full_build-shared/include
