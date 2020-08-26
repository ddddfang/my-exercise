#include "myWidget.h"

MyWidget::MyWidget(QWidget *parent) : QMainWindow(parent) {

    QPixmap pix_newpix("new.png");
    QPixmap pix_open("open.png");
    QPixmap pix_quit("quit.png");
    QAction *quit = new QAction("&Quit", this);
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);

    QMenu *file = menuBar()->addMenu("&File");
    file->addAction(quit);

}

