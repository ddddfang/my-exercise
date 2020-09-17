#include "liveWidget.h"
#include <iostream>
#include <QKeyEvent>
#include <QMoveEvent>
#include <QTime>
#include <QFont>
#include <QInputDialog>



qint64 MyIODevice::readData(char *data, qint64 maxlen) {
    qint64 readlen = 0;
    qint64 reslen = maxlen;

    while(!pFrames->isEmpty()) {
        if ((*pFrames)[0].len > reslen) {
            memcpy(data, (*pFrames)[0].data, reslen);
            (*pFrames)[0].len -= reslen;
            readlen += reslen;
            return readlen;
        } else {
            AFrame af = pFrames->takeFirst();
            memcpy(data, af.data, af.len);
            reslen -= af.len;
            readlen += af.len;
        }
    }
    std::cout << "in myIODevice::readData. read "<<readlen<<" bytes." << std::endl;
    return readlen;
}





LiveWidget::LiveWidget(QWidget *parent) : QWidget(parent) {

    this->setAttribute(Qt::WA_DeleteOnClose, 1);
    std::cout << "construct LiveWidget." << std::endl;

    //-----------------------------------------------------------------------

    QHBoxLayout *hbox_input = new QHBoxLayout();          
    this->ledit_input = new QLineEdit(this);
    this->btn_open = new QPushButton("open", this);
    this->btn_start_stop = new QPushButton("start", this);
    connect(this->btn_open, &QPushButton::clicked, this, &LiveWidget::onBtnOpen);
    connect(this->btn_start_stop, &QPushButton::clicked, this, &LiveWidget::onBtnStartStop);
    hbox_input->addWidget(this->ledit_input);
    hbox_input->addWidget(this->btn_open);
    hbox_input->addWidget(this->btn_start_stop);

    this->lbl_frame = new QLabel(this);
    //this->lbl_frame->resize({this->width(), this->height()});
    this->lbl_frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QHBoxLayout *hbox_progress = new QHBoxLayout();
    this->slider_progress = new QSlider(Qt::Horizontal);
    this->slider_progress->setRange(0, 200);
    this->lbl_progress = new QLabel("0", this);
    connect(this->slider_progress, &QSlider::valueChanged, this, &LiveWidget::onSeek);
    hbox_progress->addWidget(slider_progress);
    hbox_progress->addWidget(lbl_progress);

    //this->timer_for_pbar = new QTimer(this);
    //timer_for_pbar->isActive()
    //timer_for_pbar->start(100); //每 100 ms 触发一次 timeout
    //timer_for_pbar->stop();
    //connect(timer_for_pbar, &QTimer::timeout, this, &LiveWidget::onTimerPbar);
    /* timer */
    //lbl_time = new QLabel("", this);
    //lbl_time->setFont(QFont("Purisa", 10));
    //QTime qtime = QTime::currentTime();
    //QString stime = qtime.toString();
    //lbl_time->setText(stime);
    //startTimer(1000);   //每 1000 ms 触发一次 timerEvent, 这个 timer 应该是 mainwindow默认创建了的

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setSpacing(1);
    vbox->addLayout(hbox_input);
    vbox->addWidget(lbl_frame);
    vbox->addLayout(hbox_progress);

    this->setLayout(vbox);

    //start reader thread
    video_thread = new videoThread();
    audio_thread = new audioThread();
    connect(video_thread, SIGNAL(sigGotFrame(QImage)), this, SLOT(gotSigFrame(QImage)));
    connect(audio_thread, SIGNAL(sigGotFrame(AFrame)), this, SLOT(gotSigAFrame(AFrame)));

    QAudioFormat format;
    format.setCodec("audio/pcm");
    format.setSampleRate(48000);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);
    format.setChannelCount(2);
    format.setByteOrder(QAudioFormat::LittleEndian);

    QAudioDeviceInfo info = QAudioDeviceInfo::defaultOutputDevice();
    if (!info.isFormatSupported(format)) {
        std::cout << "audio format not support ?" << std::endl;
        format = info.nearestFormat(format);
    } else {
        std::cout << "audio format support." << std::endl;
    }

    this->aout = new QAudioOutput(format, qApp);
    this->adev = new MyIODevice(&(this->aFrames), qApp);
    //-----------------------------------------------------------------------
}

LiveWidget::~LiveWidget() {
    if (this->video_thread) {
        if (this->video_thread->isRunning()) {
            this->video_thread->myStop();
            this->video_thread->wait();    //thread join
        }
        delete this->video_thread;
    }
    if (this->audio_thread) {
        if (this->audio_thread->isRunning()) {
            this->audio_thread->myStop();
            this->audio_thread->wait();    //thread join
        }
        delete this->audio_thread;
    }
    if (this->adev) {
        delete this->adev;
    }
    if (this->aout) {
        delete this->aout;
    }
    std::cout << "destruct LiveWidget done." << std::endl;
}

void LiveWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape) {
        std::cout << "esc pressed." << std::endl;
        //退出子widget, reshow main window
        emit sigReshowMain();
        this->close();
    }
}

void LiveWidget::closeEvent(QCloseEvent * e) {
    std::cout << "you closed live widget." << std::endl;
    emit sigReshowMain();
    this->close();
}

void LiveWidget::moveEvent(QMoveEvent *e) {
    int x = e->pos().x();
    int y = e->pos().x();

    QString text = QString::number(x) + "," + QString::number(y);
    setWindowTitle(text);
}

void LiveWidget::onBtnOpen() {
    std::cout << "onBtnOpen." << std::endl;
    QString filePath = QFileDialog::getOpenFileName(this, "chose file to play", "/", "(*.*)");
    this->ledit_input->setText(filePath);
}

void LiveWidget::onBtnStartStop() {
    std::cout << "onBtnStartStop." << std::endl;
    QString path = this->ledit_input->text();
    if (!path.isEmpty()) {
        this->video_thread->setFilePath(path);
    }
    if (this->video_thread->isRunning()) {
        if (this->video_thread->isPaused()) {   //原来已经启动,但是是被pasued的状态
            this->video_thread->myPause(false); //现在解除 paused 状态
            this->audio_thread->myPause(false); //现在解除 paused 状态
            this->adev->open(QIODevice::ReadOnly);  //打开audio out
            this->aout->start(this->adev);          //

            this->btn_start_stop->setText("stop");
        } else {
            this->video_thread->myPause(true); //现在进入 paused 状态
            this->audio_thread->myPause(true); //现在进入 paused 状态
            this->aout->stop();     //关闭audio out
            this->adev->close();    //
            this->btn_start_stop->setText("start");
        }
    } else {    //原来是没有启动的状态
        this->video_thread->start();    //那么现在启动它,现在状态是已经启动,可能的操作是 stop
        this->audio_thread->start();    //那么现在启动它,现在状态是已经启动,可能的操作是 stop
        this->adev->open(QIODevice::ReadOnly);  //打开audio out
        this->aout->start(this->adev);          //
        this->btn_start_stop->setText("stop");
    }
}
void LiveWidget::onSeek(int i) {
    std::cout << "onSeek." <<i<< std::endl;

}
void LiveWidget::gotSigFrame(QImage img) {
    mImg = img.scaled(this->lbl_frame->size(), Qt::IgnoreAspectRatio);
    this->lbl_frame->setPixmap(QPixmap::fromImage(mImg));
    //this->lbl_frame->resize(mImg.size());
    this->lbl_frame->show();
}

void LiveWidget::gotSigAFrame(AFrame af) {
    //qt 播放 pcm audio
    //https://blog.csdn.net/u011283226/article/details/101024093
    //https://blog.csdn.net/caoshangpa/article/details/51224678
    //this->aFrames << af;
    this->aFrames.append(af);   //append, prepend 都可用
    ////for (AFrame afi : this->aFrames) {
    //for (int i = 0; i < this->aFrames.size(); i++) {
    //    AFrame afi = this->aFrames[i];
    //    printf("aframes size %d,LiveWidget::gotSigAFrame %p,len=%d, %x %x %x %x %x %x %x %x\n", this->aFrames.size(),afi.data, afi.len, 
    //            afi.data[0],afi.data[1],afi.data[2],afi.data[3], 
    //            afi.data[4],afi.data[5],afi.data[6],afi.data[7]);
    //}

    //QAudioFormat afmt;
    //afmt.setSampleRate(44100);
    //afmt.setChannelCount(2);
    //afmt.setSampleSize(16);
    //afmt.setCodec("audio/pcm");
    //afmt.setByteOrder(QAudioFormat::LittleEndian);
    //afmt.setSampleType(QAudioFormat::UnSignedInt);

    //QAudioOutput *aoutput = new QAudioOutput(audioFormat, this);
    //QIODevice *dev = aoutput->start();
}

//void LiveWidget::paintEvent(QPaintEvent *event)
//{
//    //QPainter painter(this);
//    //painter.setBrush(Qt::black);
//    //painter.drawRect(0, 0, this->width(), this->height());//draw black first
//    //if (mImg.size().width() <= 0)
//    //    return;
//    //QImage img = mImg.scaled(this->size(),Qt::KeepAspectRatio);
//    //int x = this->width() - img.width();
//    //int y = this->height() - img.height();
//    //x /= 2;
//    //y /= 2;
//    //painter.drawImage(QPoint(x,y),img);
//
//    this->lbl_frame->setPixmap(QPixmap::fromImage(mImg));
//    this->lbl_frame->resize(mImg.size());
//    this->lbl_frame->show();
//}
