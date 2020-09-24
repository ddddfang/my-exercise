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

#include "common.h" //AFrame and MyIODevice
#include "demuxThread.h"



class PlayerWidget : public QWidget {

    Q_OBJECT

public:
    PlayerWidget(QWidget *parent = 0);
    ~PlayerWidget();

signals:
    void sigReshowMain();

private slots:
    //-----------------
    void onBtnOpen();
    void onBtnStartStop();
    void onSeek(int);

    void gotSigVideoFrame(QImage);
    void gotSigAudioFrame(AFrame);

private:
    //---------------------
    QLineEdit *ledit_input;
    QPushButton *btn_open;
    QPushButton *btn_start_stop;
    QLabel *lbl_frame;
    QSlider *slider_progress;
    QLabel *lbl_progress;

    demuxThread *demux_thread;
    QImage mImg;            //video images, 拿到立即显示
    QList<AFrame> aFrames;  //audio frames, 拿到了需要缓存一下
    QAudioOutput *aout;
    MyIODevice *adev;

    FILE *pFilePcm;
protected:
    void keyPressEvent(QKeyEvent *e);   //处理按键事件
    void closeEvent(QCloseEvent * e);
};
