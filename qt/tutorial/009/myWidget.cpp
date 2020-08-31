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
    /* QLineEdit */
    this->ledit_name = new QLineEdit(this);
    this->ledit_phone = new QLineEdit(this);
    this->ledit_addr = new QLineEdit(this);
    QFormLayout *fl = new QFormLayout();    //专门用于表单的 layout
    fl->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    fl->addRow("Name:", ledit_name);
    fl->addRow("Phone:", ledit_phone);
    fl->addRow("Addr:", ledit_addr);

    /* QTextEdit */
    QTextEdit *edit = new QTextEdit(this);

    /* timer */
    lbl_time = new QLabel("", this);
    lbl_time->setFont(QFont("Purisa", 10));
    QTime qtime = QTime::currentTime();
    QString stime = qtime.toString();
    lbl_time->setText(stime);
    startTimer(1000);   //每 1000 ms 触发一次 timerEvent, 这个 timer 应该是 mainwindow默认创建了的

    /* QListWidget */
    this->lw = new QListWidget(this);
    this->lw->addItem("monday");
    this->lw->addItem("tuesday");
    this->lw->addItem("wednesday");
    this->lw->addItem("thrusday");

    QHBoxLayout *hbox_lw = new QHBoxLayout();          
    this->btn_add_for_lw = new QPushButton("add", this);
    this->btn_remove_for_lw = new QPushButton("remove", this);
    this->btn_rename_for_lw = new QPushButton("rename", this);
    this->btn_clear_for_lw = new QPushButton("clear", this);
    btn_add_for_lw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn_remove_for_lw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn_rename_for_lw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    btn_clear_for_lw->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(btn_add_for_lw, &QPushButton::clicked, this, &MyWidget::onBtnAddLw);
    connect(btn_remove_for_lw, &QPushButton::clicked, this, &MyWidget::onBtnRemoveLw);
    connect(btn_rename_for_lw, &QPushButton::clicked, this, &MyWidget::onBtnRenameLw);
    connect(btn_clear_for_lw, &QPushButton::clicked, this, &MyWidget::onBtnClearLw);
    hbox_lw->addWidget(btn_add_for_lw);
    hbox_lw->addWidget(btn_remove_for_lw);
    hbox_lw->addWidget(btn_rename_for_lw);
    hbox_lw->addWidget(btn_clear_for_lw);

    /* QProgressBar timer */
    QHBoxLayout *hbox_pbar = new QHBoxLayout();          
    this->progress_bar = new QProgressBar();
    this->timer_for_pbar = new QTimer(this);
    this->btn_start_for_pbar = new QPushButton("start", this);
    this->btn_stop_for_pbar = new QPushButton("stop", this);
    connect(timer_for_pbar, &QTimer::timeout, this, &MyWidget::onTimerPbar);
    connect(btn_start_for_pbar, &QPushButton::clicked, this, &MyWidget::onBtnStart);
    connect(btn_stop_for_pbar, &QPushButton::clicked, this, &MyWidget::onBtnStop);
    hbox_pbar->addWidget(progress_bar);
    hbox_pbar->addWidget(btn_start_for_pbar);
    hbox_pbar->addWidget(btn_stop_for_pbar);
    this->progress = 0;

    /* QSlider */
    QHBoxLayout *hbox_score = new QHBoxLayout();          
    QSlider *slider_score = new QSlider(Qt::Horizontal);
    slider_score->setRange(0, 200);
    QLabel *lbl_score = new QLabel("0", this);
    hbox_score->addWidget(slider_score);
    hbox_score->addWidget(lbl_score);
    //connect 前两个参数是 谁 发送什么信号, 后两个参数是 谁 用什么函数处理信号
    //since the setNum() method is overloaded, we use a static_cast to choose the correct method
    connect(slider_score, &QSlider::valueChanged, lbl_score, static_cast<void (QLabel::*)(int)>(&QLabel::setNum));

    /* QSpinBox */
    QHBoxLayout *hbox_score2 = new QHBoxLayout();  
    QSpinBox *spbox_score2 = new QSpinBox(this);
    QLabel *lbl_score2 = new QLabel("0", this);
    hbox_score2->addWidget(spbox_score2);
    hbox_score2->addWidget(lbl_score2);
    connect(spbox_score2, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            lbl_score2, static_cast<void (QLabel::*)(int)>(&QLabel::setNum));

    /* QComboBox */
    QHBoxLayout *hbox_distri = new QHBoxLayout();          
    QStringList items = {"Arch", "RedHat", "Xubuntu", "Debian"};
    QComboBox *combo_distri = new QComboBox();
    combo_distri->addItems(items);
    QLabel *lbl_distri = new QLabel("Arch", this);
    hbox_distri->addWidget(combo_distri);
    hbox_distri->addSpacing(15);
    hbox_distri->addWidget(lbl_distri);
    connect(combo_distri, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::activated), lbl_distri, &QLabel::setText);

    /* QCheckBox */
    QHBoxLayout *hbox_for_check = new QHBoxLayout();  
    this->checkb = new QCheckBox("test check", this);
    checkb->setCheckState(Qt::Checked);
    this->lbl_for_check = new QLabel("checked 1", this);
    hbox_for_check->addWidget(checkb, 0, Qt::AlignLeft | Qt::AlignTop);
    hbox_for_check->addWidget(lbl_for_check, 0, Qt::AlignRight | Qt::AlignVCenter);
    connect(checkb, &QCheckBox::stateChanged, this, &MyWidget::onCheckBoxStateChange);

    /* QPushButton */
    QHBoxLayout *hbox = new QHBoxLayout();          
    //QPushButton *btn_yes = new QPushButton("yes", this);
    //QPushButton *btn_no = new QPushButton("no", this);
    this->btn_yes = new QPushButton("yes", this);
    this->btn_no = new QPushButton("no", this);
    btn_yes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //这个按钮的宽高都是扩展的,所以size自动变化
    btn_no->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(btn_yes, &QPushButton::clicked, this, &MyWidget::onBtnYes);
    connect(btn_no, &QPushButton::clicked, this, &MyWidget::onBtnNo);
    hbox->addWidget(btn_yes);
    hbox->addWidget(btn_no);

    /* QPixMap on QLabel */
    QLabel *lbl_pixmap = new QLabel(this);
    QPixmap pixmap("./te.jpg");
    lbl_pixmap->setPixmap(pixmap);  //在 label 上显示 图片

    /* 似乎显示有点问题 */
    //QFrame *frame1 = new QFrame(this);
    //frame1->setFrameShape(QFrame::Box); //QFrame::Box
    //QFrame *frame2 = new QFrame(this);
    //frame2->setFrameShape(QFrame::Box);//QFrame::StyledPanel
    //QFrame *frame3 = new QFrame(this);
    //frame1->setFrameShape(QFrame::Box);
    //QSplitter *splt1 = new QSplitter(Qt::Horizontal, this);
    //splt1->addWidget(frame1);
    //splt1->addWidget(frame2);
    //QSplitter *splt2 = new QSplitter(Qt::Vertical, this);
    //splt2->addWidget(splt1);
    //splt2->addWidget(frame3);
    //QList<int> sizes = {500, 100};
    //splt2->setSizes(sizes);

    QVBoxLayout *vbox = new QVBoxLayout();          
    vbox->setSpacing(1);
    vbox->addLayout(fl);
    vbox->addWidget(edit);
    vbox->addWidget(lbl_time);
    vbox->addWidget(lw);
    vbox->addLayout(hbox_lw);
    vbox->addLayout(hbox_pbar);
    vbox->addLayout(hbox_score);
    vbox->addLayout(hbox_score2);
    vbox->addLayout(hbox_distri);
    vbox->addLayout(hbox_for_check);
    vbox->addLayout(hbox);
    vbox->addWidget(lbl_pixmap);
    //vbox->addWidget(splt2);

    QWidget *cw = new QWidget(this);
    cw->setLayout(vbox);
    setCentralWidget(cw);

    //-----------------------------------------------------------------------

    initStatusBar();
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
    QString filePath = QFileDialog::getOpenFileName(this, "chose file to play", "/", "(*.264 *.avi *.mov *.flv *.mkv *.ts *.mp3)");
    std::cout << "in MyWidget::onActOpen() :" << filePath.toStdString() <<std::endl;
}

void MyWidget::toggleStatusBar() {
    if (this->act_toggle_view->isChecked()) {
        std::cout << "in MyWidget::toggleStatusBar() 1." << std::endl;
        statusBar()->show();
        connect(btn_yes, &QPushButton::clicked, this, &MyWidget::onBtnYes);
        connect(btn_no, &QPushButton::clicked, this, &MyWidget::onBtnNo);
    } else {
        std::cout << "in MyWidget::toggleStatusBar() 0." << std::endl;
        statusBar()->hide();
        disconnect(btn_yes, &QPushButton::clicked, this, &MyWidget::onBtnYes);  //停止对 push button 的响应
        disconnect(btn_no, &QPushButton::clicked, this, &MyWidget::onBtnNo);    //停止对 push button 的响应
    }
}

void MyWidget::onBtnYes() {
    std::cout << "MyWidget::onBtnYes()" << std::endl;
    std::cout << "name:" << this->ledit_name->text().toStdString() << std::endl;
    std::cout << "phone:" << this->ledit_phone->text().toStdString() << std::endl;
    std::cout << "addr:" << this->ledit_addr->text().toStdString() << std::endl;
}

void MyWidget::onBtnNo() {
    std::cout << "MyWidget::onBtnNo()" << std::endl;
}

void MyWidget::onCheckBoxStateChange() {

    if (this->checkb->isChecked()) {
        std:: cout << "checkb->isChecked 1" << std::endl;
        this->lbl_for_check->setText("checked 1");
    } else {
        std:: cout << "checkb->isChecked 0" << std::endl;
        this->lbl_for_check->setText("checked 0");
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

void MyWidget::timerEvent(QTimerEvent *e) {
    Q_UNUSED(e);

    QTime qtime = QTime::currentTime();
    QString stime = qtime.toString();
    lbl_time->setText(stime);
}

void MyWidget::onBtnAddLw() {
    std::cout << "onBtnAddLw." << std::endl;
    QString input_item = QInputDialog::getText(this, "Item", "Enter new item");
    input_item = input_item.simplified();   //去掉可能的无用的空格
    if (!input_item.isEmpty()) {
        this->lw->addItem(input_item);
        int r = this->lw->count() - 1;
        this->lw->setCurrentRow(r);
    }
}

void MyWidget::onBtnRemoveLw() {
    std::cout << "onBtnRemoveLw." << std::endl;
    //QListWidgetItem *cur_item = this->lw->currentItem();
    int cur_row = this->lw->currentRow();

    if (cur_row != -1) {
        QListWidgetItem *item = this->lw->takeItem(cur_row);    //将指定行对应的 row 从 listwidget 移除并返回
        delete item;
    }
}

void MyWidget::onBtnRenameLw() {
    QListWidgetItem *cur_item = this->lw->currentItem();
    
    QString cur_text = cur_item->text();
    QString new_text = QInputDialog::getText(this, "Item", "Enter new item", QLineEdit::Normal, cur_text);
    std::cout << "onBtnRenameLw ("<<cur_text.toStdString()<<")<-("<<new_text.toStdString()<<")." << std::endl;
    new_text = new_text.simplified();

    if (!new_text.isEmpty()) {
        int cur_row = this->lw->row(cur_item);
        QListWidgetItem *item = this->lw->takeItem(cur_row);    //将指定行对应的 row 从 listwidget 移除并返回
        delete item;
        this->lw->insertItem(cur_row, new_text);    //在指定行前插入新的行
        this->lw->setCurrentRow(cur_row);
    }
}

void MyWidget::onBtnClearLw() {
    std::cout << "onBtnClearLw." << std::endl;

    if (this->lw->count() > 0) {
        this->lw->clear();
    }
}

void MyWidget::onTimerPbar() {  //update progress
    std::cout << "onTimerPbar." << std::endl;

    progress++;
    if (progress <= 100) {  //进度条默认最大值就是100
        progress_bar->setValue(progress);
    } else {
        timer_for_pbar->stop();
        btn_start_for_pbar->setEnabled(true);
        btn_stop_for_pbar->setEnabled(false);
    }
}

void MyWidget::onBtnStart() {   //start timer
    std::cout << "onBtnStart." << std::endl;
    if (progress >= 100) {
        progress = 0;
        progress_bar->setValue(progress);
    }

    if (!timer_for_pbar->isActive()) {
        btn_start_for_pbar->setEnabled(false);
        btn_stop_for_pbar->setEnabled(true);
        timer_for_pbar->start(100); //每 100 ms 触发一次 timeout
    }
}

void MyWidget::onBtnStop() {    //stop timer
    std::cout << "onBtnStop." << std::endl;
    if (timer_for_pbar->isActive()) {
        btn_stop_for_pbar->setEnabled(false);
        btn_start_for_pbar->setEnabled(true);
        timer_for_pbar->stop();
    }

}

