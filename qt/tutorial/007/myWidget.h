#pragma once

#include <QApplication>
#include <QWidget>
#include <QIcon>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>


class MyWidget : public QWidget {

    //如果要定义自己的 signal/slots 需要加上这个宏
    Q_OBJECT

public:
    MyWidget(QWidget *parent = 0);

private slots:
    void onPlus();
    void onMinus();

private:
    QLabel *lbl;
};
