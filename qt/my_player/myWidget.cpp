#include "myWidget.h"
#include <iostream>
#include <QKeyEvent>
#include <QMoveEvent>
#include <QTime>
#include <QFont>
#include <QInputDialog>

MyWidget::MyWidget(QWidget *parent) : QMainWindow(parent) {

    initToolBar();
    initMenuBar();

    //-----------------------------------------------------------------------

    QHBoxLayout *hbox_input = new QHBoxLayout();          
    this->ledit_input = new QLineEdit(this);
    this->btn_open = new QPushButton("open", this);
    this->btn_start_stop = new QPushButton("start", this);
    connect(this->btn_open, &QPushButton::clicked, this, &MyWidget::onBtnOpen);
    connect(this->btn_start_stop, &QPushButton::clicked, this, &MyWidget::onBtnStartStop);
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
    connect(this->slider_progress, &QSlider::valueChanged, this, &MyWidget::onSeek);
    hbox_progress->addWidget(slider_progress);
    hbox_progress->addWidget(lbl_progress);

    //this->timer_for_pbar = new QTimer(this);
    //timer_for_pbar->isActive()
    //timer_for_pbar->start(100); //每 100 ms 触发一次 timeout
    //timer_for_pbar->stop();
    //connect(timer_for_pbar, &QTimer::timeout, this, &MyWidget::onTimerPbar);
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

    QWidget *cw = new QWidget(this);
    cw->setLayout(vbox);
    setCentralWidget(cw);

    //start reader thread
    reader = new readerThread();
    connect(reader, SIGNAL(sigGotFrame(QImage)), this, SLOT(gotSigFrame(QImage)));
    b_started = false;
    //-----------------------------------------------------------------------

    initStatusBar();
}

MyWidget::~MyWidget() {
    if (this->reader) {
        if (this->reader->isRunning()) {
            this->reader->stop();
            this->reader->wait();    //thread join
        }
        delete this->reader;
    }
}

void MyWidget::initToolBar() {
    QPixmap pix_new("new.png");
    QPixmap pix_open("open.png");
    QPixmap pix_quit("quit.png");

    this->toolbar = addToolBar("main toolbar");
    this->act_new = this->toolbar->addAction(QIcon(pix_new), "New File");
    this->act_open = this->toolbar->addAction(QIcon(pix_open), "Open File");
    this->act_quit = this->toolbar->addAction(QIcon(pix_quit), "Quit");
    connect(this->act_new, &QAction::triggered, this, &MyWidget::onActNew);
    connect(this->act_open, &QAction::triggered, this, &MyWidget::onActOpen);
    connect(this->act_quit, &QAction::triggered, qApp, &QApplication::quit);
}

void MyWidget::initMenuBar () {
    menuFile = menuBar()->addMenu("&File");
    menuFile->addAction(this->act_new);
    menuFile->addAction(this->act_open);
    menuFile->addSeparator();
    menuFile->addAction(this->act_quit);

    this->act_toggle_view = new QAction("&View Status", this);
    this->act_toggle_view->setCheckable(true);
    this->act_toggle_view->setChecked(true);
    connect(this->act_toggle_view, &QAction::triggered, this, &MyWidget::toggleStatusBar);
    menuView = menuBar()->addMenu("&View");
    menuView->addAction(this->act_toggle_view);

    //qApp->setAttribute(Qt::AA_DontShowIconsInMenus, true); //将 AA_DontShowIconsInMenus 属性设为 false
}

void MyWidget::initStatusBar() {
    statusBar()->showMessage("Ready");
}

void MyWidget::onActNew() {
    std::cout << "in MyWidget::onActNew()." << std::endl;
}

void MyWidget::onActOpen() {
    //QString filePath = QFileDialog::getOpenFileName(this, "chose file to play", "/", "(*.264 *.avi *.mov *.flv *.mkv *.ts *.mp3)");
    QString filePath = QFileDialog::getOpenFileName(this, "chose file to play", "/", "(*.*)");
    std::cout << "in MyWidget::onActOpen() :" << filePath.toStdString() <<std::endl;

    this->ledit_input->setText(filePath);
}

void MyWidget::toggleStatusBar() {
    if (this->act_toggle_view->isChecked()) {
        std::cout << "in MyWidget::toggleStatusBar() 1." << std::endl;
        statusBar()->show();
    } else {
        std::cout << "in MyWidget::toggleStatusBar() 0." << std::endl;
        statusBar()->hide();
    }
}

void MyWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape) {
        std::cout << "esc pressed." << std::endl;
        qApp->quit();
    }
}

void MyWidget::moveEvent(QMoveEvent *e) {
    int x = e->pos().x();
    int y = e->pos().x();

    QString text = QString::number(x) + "," + QString::number(y);
    setWindowTitle(text);
}

void MyWidget::onBtnOpen() {
    std::cout << "onBtnOpen." << std::endl;
    QString filePath = QFileDialog::getOpenFileName(this, "chose file to play", "/", "(*.*)");
    this->ledit_input->setText(filePath);
    this->reader->setFilePath(filePath);
}
void MyWidget::onBtnStartStop() {
    std::cout << "onBtnStartStop." << std::endl;
    if (b_started) {
        b_started = false;
        this->btn_start_stop->setText("start");
        if (this->reader->isRunning()) {
            this->reader->pause(true);
        }
    } else {
        b_started = true;
        this->btn_start_stop->setText("stop");
        if (!this->reader->isRunning()) {
            this->reader->start();
        } else {
            this->reader->pause(false);
        }
    }
}
void MyWidget::onSeek(int i) {
    std::cout << "onSeek." <<i<< std::endl;

}
void MyWidget::gotSigFrame(QImage img) {
    mImg = img.scaled(this->lbl_frame->size(), Qt::IgnoreAspectRatio);
    this->lbl_frame->setPixmap(QPixmap::fromImage(mImg));
    //this->lbl_frame->resize(mImg.size());
    this->lbl_frame->show();
}

//void MyWidget::paintEvent(QPaintEvent *event)
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
