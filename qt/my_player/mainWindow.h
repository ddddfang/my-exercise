#pragma once

#include <QApplication>
#include <QWidget>
#include <QFileDialog>
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
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QProgressBar>
#include <QTimer>
#include <QPixmap>
#include <QSplitter>
#include <QPainter>

#include "apps/live/liveWidget.h"

//class MyWidget : public QWidget {
class MainWidget : public QMainWindow {   //主窗口我们选择从 QMainWindow 继承,这样可以有菜单栏 QMainWindow::menuBar() 可用

    Q_OBJECT

public:
    MainWidget(QWidget *parent = 0);
    ~MainWidget();

private slots:
    void onActOpen();
    void onActNew();
    void toggleStatusBar();
    void reShow();

    //-----------------
    void onBtnLive();
    void onBtnPlayer();

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

    //---------------------
    QPushButton *btn_live;
    QPushButton *btn_player;

    LiveWidget *live;

protected:
    void keyPressEvent(QKeyEvent *e);   //处理按键事件
    void moveEvent(QMoveEvent *e);      //处理窗口移动事件
    //void paintEvent(QPaintEvent *e);
};
