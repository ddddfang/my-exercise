#include "playerWidget.h"
#include <iostream>
#include <QKeyEvent>
#include <QMoveEvent>
#include <QTime>
#include <QFont>
#include <QInputDialog>



PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent) {

    this->setAttribute(Qt::WA_DeleteOnClose, 1);
    std::cout << "construct PlayerWidget." << std::endl;

    //-----------------------------------------------------------------------

    QHBoxLayout *hbox_input = new QHBoxLayout();          
    this->ledit_input = new QLineEdit(this);
    this->btn_open = new QPushButton("open", this);
    this->btn_start_stop = new QPushButton("start", this);
    connect(this->btn_open, &QPushButton::clicked, this, &PlayerWidget::onBtnOpen);
    connect(this->btn_start_stop, &QPushButton::clicked, this, &PlayerWidget::onBtnStartStop);
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
    connect(this->slider_progress, &QSlider::valueChanged, this, &PlayerWidget::onSeek);
    hbox_progress->addWidget(slider_progress);
    hbox_progress->addWidget(lbl_progress);

    QVBoxLayout *vbox = new QVBoxLayout();
    vbox->setSpacing(1);
    vbox->addLayout(hbox_input);
    vbox->addWidget(lbl_frame);
    vbox->addLayout(hbox_progress);

    this->setLayout(vbox);

    //start reader thread
    demux_thread = new demuxThread();
    connect(demux_thread, SIGNAL(sigGotVideoFrame(QImage)), this, SLOT(gotSigVideoFrame(QImage)));
    connect(demux_thread, SIGNAL(sigGotAudioFrame(AFrame)), this, SLOT(gotSigAudioFrame(AFrame)));

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

    this->pFilePcm = fopen("test_out.pcm", "wb");
    //-----------------------------------------------------------------------
}

PlayerWidget::~PlayerWidget() {
    fclose(this->pFilePcm);

    if (this->demux_thread) {
        if (this->demux_thread->isRunning()) {
            this->demux_thread->myStop();
            this->demux_thread->wait();    //thread join
        }
        delete this->demux_thread;
    }
    if (this->adev) {
        delete this->adev;
    }
    if (this->aout) {
        delete this->aout;
    }
    std::cout << "destruct PlayerWidget done." << std::endl;
}

void PlayerWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape) {
        std::cout << "esc pressed." << std::endl;
        //退出子widget, reshow main window
        emit sigReshowMain();
        this->close();
    }
}

void PlayerWidget::closeEvent(QCloseEvent * e) {
    std::cout << "you closed PlayerWidget widget." << std::endl;
    Q_UNUSED(e);
    emit sigReshowMain();
    this->close();
}

void PlayerWidget::onBtnOpen() {
    std::cout << "onBtnOpen." << std::endl;
    QString filePath = QFileDialog::getOpenFileName(this, "chose file to play", "/home/fang", "(*.264 *.avi *.mov *.flv *.mkv *.ts *.mp3)");
    this->ledit_input->setText(filePath);
}

void PlayerWidget::onBtnStartStop() {
    std::cout << "onBtnStartStop." << std::endl;
    QString path = this->ledit_input->text();
    if (!path.isEmpty()) {
        this->demux_thread->setFilePath(path);
    }
    if (this->demux_thread->isRunning()) {
        if (this->demux_thread->isPaused()) {   //原来已经启动,但是是被pasued的状态
            this->demux_thread->myPause(false); //现在解除 paused 状态
            //this->adev->open(QIODevice::ReadOnly);  //打开audio out
            //this->aout->start(this->adev);          //

            this->btn_start_stop->setText("stop");
        } else {
            this->demux_thread->myPause(true); //现在进入 paused 状态
            //this->aout->stop();     //关闭audio out
            //this->adev->close();    //
            this->btn_start_stop->setText("start");
        }
    } else {    //原来是没有启动的状态
        this->demux_thread->start();    //那么现在启动它,现在状态是已经启动,可能的操作是 stop
        //this->adev->open(QIODevice::ReadOnly);  //打开audio out
        //this->aout->start(this->adev);          //
        this->btn_start_stop->setText("stop");
    }
}

void PlayerWidget::onSeek(int i) {
    std::cout << "onSeek." <<i<< std::endl;

}

void PlayerWidget::gotSigVideoFrame(QImage img) {
    mImg = img.scaled(this->lbl_frame->size(), Qt::IgnoreAspectRatio);
    this->lbl_frame->setPixmap(QPixmap::fromImage(mImg));
    //this->lbl_frame->resize(mImg.size());
    this->lbl_frame->show();
}

void PlayerWidget::gotSigAudioFrame(AFrame af) {
    //qt 播放 pcm audio
    //https://blog.csdn.net/u011283226/article/details/101024093
    //https://blog.csdn.net/caoshangpa/article/details/51224678
    //this->aFrames << af;
    //this->aFrames.append(af);   //append, prepend 都可用
    fwrite(af.data, 1, af.len, this->pFilePcm);
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

