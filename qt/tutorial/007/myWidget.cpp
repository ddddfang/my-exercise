#include "myWidget.h"

MyWidget::MyWidget(QWidget *parent) : QWidget(parent) {
    QPushButton *btnPls = new QPushButton("+", this);
    QPushButton *btnMin = new QPushButton("-", this);
    lbl = new QLabel("0", this);

    QGridLayout *grid = new QGridLayout(this);
    grid->addWidget(btnPls, 0, 0);
    grid->addWidget(btnMin, 0, 2);
    grid->addWidget(lbl, 1, 1);
    setLayout(grid);

    //qApp 是一个全局指针,指向 QApplication 对象,在 QApplication 头文件中定义
    //connect(btnQuit, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(btnPls, &QPushButton::clicked, this, &MyWidget::onPlus);
    connect(btnMin, &QPushButton::clicked, this, &MyWidget::onMinus);
}

void MyWidget::onPlus() {
    int val = lbl->text().toInt();
    val++;
    lbl->setText(QString::number(val));
}

void MyWidget::onMinus() {
    int val = lbl->text().toInt();
    val--;
    lbl->setText(QString::number(val));
}
