#include "myWidget.h"
#include <iostream>

MyWidget::MyWidget(QWidget *parent) : QMainWindow(parent) {

    initToolBar();
    initMenuBar();

    //-----------------------------------------------------------------------
    //QLineEdit *ledit_name = new QLineEdit(this);
    //QLineEdit *ledit_phone = new QLineEdit(this);
    //QLineEdit *ledit_addr = new QLineEdit(this);
    //QFormLayout *fl = new QFormLayout();    //专门用于表单的 layout
    //fl->setLabelAlignment(Qt::AlignRight|Qt::AlignVCenter);
    //fl->addRow("Name:", ledit_name);
    //fl->addRow("Phone:", ledit_phone);
    //fl->addRow("Addr:", ledit_addr);

    //QTextEdit *edit = new QTextEdit(this);
    ////setCentralWidget(edit);

    //QListWidget *lw = new QListWidget(this);
    //lw->addItem("monday");
    //lw->addItem("tuesday");
    //lw->addItem("wednesday");
    //lw->addItem("thrusday");

    //QHBoxLayout *hbox = new QHBoxLayout();          
    //QPushButton *btn_yes = new QPushButton("yes", this);
    //btn_yes->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); //这个按钮的宽高都是扩展的,所以size自动变化
    //QPushButton *btn_no = new QPushButton("no", this);
    //btn_no->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    ////hbox->addStretch(1);    //可伸缩的空格
    ////hbox->addWidget(lw, 1);    //比例因子为 1
    //hbox->addWidget(btn_yes, 1);    //比例因子为 1
    //hbox->addWidget(btn_no, 0, Qt::AlignRight); //比例因子为 0 保持default大小
    ////hbox->addStretch(1);    //可伸缩的空格

    ////new QVBoxLayout(this); 通过this指针设置了QLayout的所属组件，也就是隐式的设置了这个QWidget的布局,
    ////所以 cw->setLayout(vbox); 后会报告 "QLayout: Attempting to add QLayout "" to MyWidget "", which already has a layout"
    ////的警告,去掉 this 指针可以避免这种情况
    ////参看 https://www.cnblogs.com/csuftzzk/p/qlayout_warnings.html

    //QVBoxLayout *vbox = new QVBoxLayout();          
    //vbox->setSpacing(1);

    //vbox->addLayout(fl);
    //vbox->addWidget(edit);
    //vbox->addWidget(lw);
    ////vbox->addSpacing(15);
    //vbox->addLayout(hbox);

    //QWidget *cw = new QWidget(this);
    //cw->setLayout(vbox);
    //setCentralWidget(cw);

    //------------------------------------
    //QGridLayout *gl = new QGridLayout();
    //gl->setSpacing(2);
    //QList<QString> lbls= {
    //    "7", "8", "9", "/",
    //    "4", "5", "6", "*",
    //    "1", "2", "3", "-",
    //    "0", ".", "=", "+"
    //};
    //int pos = 0;
    //for (int i = 0; i < 4; i++) {
    //    for (int j = 0; j < 4; j++) {
    //        QPushButton *btn = new QPushButton(lbls[pos], this);
    //        btn->setFixedSize(40, 40);
    //        gl->addWidget(btn, i, j);
    //        pos++;
    //    }
    //}

    //QWidget *cw = new QWidget(this);
    //cw->setLayout(gl);
    //setCentralWidget(cw);

    //------------------------------------
    QGridLayout *gl = new QGridLayout();
    gl->setVerticalSpacing(15);
    gl->setHorizontalSpacing(10);

    QLabel *lbl_title = new QLabel("Title", this);
    lbl_title->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    gl->addWidget(lbl_title, 0, 0, 1, 1);

    QLineEdit *ledit_title = new QLineEdit(this);
    gl->addWidget(ledit_title, 0, 1, 1, 1);

    QLabel *lbl_author = new QLabel("Author", this);
    lbl_title->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    gl->addWidget(lbl_author, 1, 0, 1, 1);

    QLineEdit *ledit_author = new QLineEdit(this);
    gl->addWidget(ledit_author, 1, 1, 1, 1);

    QLabel *lbl_review = new QLabel("Review", this);
    lbl_title->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    gl->addWidget(lbl_review, 2, 0, 1, 1);

    QTextEdit *ledit_review = new QTextEdit(this);
    gl->addWidget(ledit_review, 2, 1, 3, 1);    //QTextEdit 占据3行1列

    QWidget *cw = new QWidget(this);
    cw->setLayout(gl);
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
    std::cout << "in MyWidget::onActOpen()." << std::endl;
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

