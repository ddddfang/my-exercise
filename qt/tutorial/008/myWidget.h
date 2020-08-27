#pragma once

#include <QApplication>
#include <QWidget>
#include <QIcon>
#include <QFrame>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>

#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMainWindow>
#include <QStatusBar>

#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>


//class MyWidget : public QWidget {
class MyWidget : public QMainWindow {   //主窗口我们选择从 QMainWindow 继承,这样可以有菜单栏 QMainWindow::menuBar() 可用

    Q_OBJECT

public:
    MyWidget(QWidget *parent = 0);

private slots:
    void onActOpen();
    void onActNew();
    void toggleStatusBar();

private:
    void initToolBar();
    void initMenuBar();
    void initStatusBar();

    QToolBar *toolbar;
    QMenu *menuFile;
    QMenu *menuView;

    QAction *act_new;
    QAction *act_open;
    QAction *act_quit;
    QAction *act_toggle_view;
};
