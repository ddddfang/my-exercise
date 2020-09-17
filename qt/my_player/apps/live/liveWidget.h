#pragma once

#include <QApplication>
#include <QWidget>
#include <QFileDialog>
#include <QIcon>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMainWindow>
#include <QStatusBar>

#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QTimer>
#include <QPixmap>
#include <QSplitter>
#include <QPainter>

#include <QAudioFormat>
#include <QAudioOutput>

#include "videoThread.h"
#include "audioThread.h"


//qt 播放 pcm
class MyIODevice : public QIODevice {
public:
    MyIODevice(QList<AFrame> *audioFrames, QObject *parent = nullptr) : QIODevice(parent) {
        pFrames = audioFrames;
    }
    ~MyIODevice() { }

    virtual qint64 readData(char *data, qint64 maxlen);

    virtual qint64 writeData(const char *data, qint64 len) {
        Q_UNUSED(data);
        Q_UNUSED(len);
        return 0;
    }

private:
    QList<AFrame> *pFrames;
};





class LiveWidget : public QWidget {

    Q_OBJECT

public:
    LiveWidget(QWidget *parent = 0);
    ~LiveWidget();

signals:
    void sigReshowMain();

private slots:
    //-----------------
    void onBtnOpen();
    void onBtnStartStop();
    void onSeek(int);

    void gotSigFrame(QImage);
    void gotSigAFrame(AFrame);

private:
    //---------------------
    QLineEdit *ledit_input;
    QPushButton *btn_open;
    QPushButton *btn_start_stop;
    QLabel *lbl_frame;
    QSlider *slider_progress;
    QLabel *lbl_progress;

    videoThread *video_thread;
    audioThread *audio_thread;
    QImage mImg;            //video images, 拿到立即显示
    QList<AFrame> aFrames;  //audio frames, 拿到了需要缓存一下
    QAudioOutput *aout;
    MyIODevice *adev;

protected:
    void keyPressEvent(QKeyEvent *e);   //处理按键事件
    void moveEvent(QMoveEvent *e);      //处理窗口移动事件
    void closeEvent(QCloseEvent * e);
    //void paintEvent(QPaintEvent *e);
};
