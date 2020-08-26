#pragma once

#include <QApplication>
#include <QWidget>
#include <QIcon>
#include <QFrame>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>

#include <QMenu>
#include <QMenuBar>
#include <QMainWindow>


//class MyWidget : public QWidget {
class MyWidget : public QMainWindow {   //主窗口我们选择从 QMainWindow 继承,这样可以有菜单栏 QMainWindow::menuBar() 可用

public:
    MyWidget(QWidget *parent = 0);

};
