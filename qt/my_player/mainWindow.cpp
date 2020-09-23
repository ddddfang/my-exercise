#include "mainWindow.h"
#include <iostream>
#include <QKeyEvent>
#include <QMoveEvent>
#include <QTime>
#include <QFont>
#include <QInputDialog>

MainWidget::MainWidget(QWidget *parent) : QMainWindow(parent) {

    initToolBar();
    initMenuBar();

    //-----------------------------------------------------------------------
    qRegisterMetaType<AFrame>("AFrame");    //这里注册我们使用的 AFrame 类,不然在 connect() 的时候 qt 不认

    this->btn_live = new QPushButton("live", this);
    this->btn_player = new QPushButton("player", this);
    this->btn_gplayer = new QPushButton("gstreamer player", this);
    this->btn_live->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->btn_player->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->btn_gplayer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(this->btn_live, &QPushButton::clicked, this, &MainWidget::onBtnLive);
    connect(this->btn_player, &QPushButton::clicked, this, &MainWidget::onBtnPlayer);
    connect(this->btn_gplayer, &QPushButton::clicked, this, &MainWidget::onBtnGPlayer);

    QVBoxLayout *vbox = new QVBoxLayout();          
    vbox->setSpacing(1);
    vbox->addWidget(this->btn_live);
    vbox->addWidget(this->btn_player);
    vbox->addWidget(this->btn_gplayer);


    QWidget *cw = new QWidget(this);
    cw->setLayout(vbox);
    setCentralWidget(cw);

    //-----------------------------------------------------------------------

    initStatusBar();
}

MainWidget::~MainWidget() {
}

void MainWidget::initToolBar() {
    QPixmap pix_new("./res/new.png");
    QPixmap pix_open("./res/open.png");
    QPixmap pix_quit("./res/quit.png");

    this->toolbar = addToolBar("main toolbar");
    this->act_new = this->toolbar->addAction(QIcon(pix_new), "New File");
    this->act_open = this->toolbar->addAction(QIcon(pix_open), "Open File");
    this->act_quit = this->toolbar->addAction(QIcon(pix_quit), "Quit");
    connect(this->act_new, &QAction::triggered, this, &MainWidget::onActNew);
    connect(this->act_open, &QAction::triggered, this, &MainWidget::onActOpen);
    connect(this->act_quit, &QAction::triggered, qApp, &QApplication::quit);
}

void MainWidget::initMenuBar () {
    menuFile = menuBar()->addMenu("&File");
    menuFile->addAction(this->act_new);
    menuFile->addAction(this->act_open);
    menuFile->addSeparator();
    menuFile->addAction(this->act_quit);

    this->act_toggle_view = new QAction("&View Status", this);
    this->act_toggle_view->setCheckable(true);
    this->act_toggle_view->setChecked(true);
    connect(this->act_toggle_view, &QAction::triggered, this, &MainWidget::toggleStatusBar);
    menuView = menuBar()->addMenu("&View");
    menuView->addAction(this->act_toggle_view);
}

void MainWidget::initStatusBar() {
    statusBar()->showMessage("Ready");
}

void MainWidget::onActNew() {
    std::cout << "in MainWidget::onActNew()." << std::endl;
}

void MainWidget::onActOpen() {
    QString filePath = QFileDialog::getOpenFileName(this, "chose file to play", "/", "(*.264 *.avi *.mov *.flv *.mkv *.ts *.mp3)");
    std::cout << "in MainWidget::onActOpen() :" << filePath.toStdString() <<std::endl;
}

void MainWidget::toggleStatusBar() {
    if (this->act_toggle_view->isChecked()) {
        std::cout << "in MainWidget::toggleStatusBar() 1." << std::endl;
        statusBar()->show();
    } else {
        std::cout << "in MainWidget::toggleStatusBar() 0." << std::endl;
        statusBar()->hide();
    }
}

void MainWidget::keyPressEvent(QKeyEvent *e) {
    if (e->key() == Qt::Key_Escape) {
        std::cout << "esc pressed." << std::endl;
        qApp->quit();
    }
}

void MainWidget::moveEvent(QMoveEvent *e) {
    int x = e->pos().x();
    int y = e->pos().x();

    QString text = QString::number(x) + "," + QString::number(y);
    setWindowTitle(text);
}

void MainWidget::reShow() {
    this->show();
}

void MainWidget::onBtnLive() {
    std::cout << "onBtnLive." << std::endl;

    this->hide();                       //主界面关闭
    //this->live = new LiveWidget(this);    //新建子界面,这里给定了this作为 parent,parent指定了layout,会导致子widget显示不出来
    this->live = new LiveWidget();          //新建子界面,去掉this就行了
    connect(this->live, SIGNAL(sigReshowMain()), this, SLOT(reShow())); //当点击子界面时，调用主界面的reshow()函数

    this->live->resize(800, 600);
    this->live->move(300, 300);
    this->live->setWindowTitle("live");
    this->live->show();                 //子界面出现
}

void MainWidget::onBtnPlayer() {
    std::cout << "onBtnPlayer." << std::endl;
    this->hide();                       //主界面关闭
    this->player = new PlayerWidget();  //新建子界面
    connect(this->player, SIGNAL(sigReshowMain()), this, SLOT(reShow())); //当点击子界面时，调用主界面的reshow()函数

    this->player->resize(800, 600);
    this->player->move(300, 300);
    this->player->setWindowTitle("player");
    this->player->show();                 //子界面出现
}

void MainWidget::onBtnGPlayer() {
    std::cout << "onBtnGPlayer." << std::endl;
}

