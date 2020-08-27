#include "myWidget.h"
#include <iostream>

MyWidget::MyWidget(QWidget *parent) : QMainWindow(parent) {

    initToolBar();
    initMenuBar();

    QTextEdit *edit = new QTextEdit(this);
    setCentralWidget(edit);

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

