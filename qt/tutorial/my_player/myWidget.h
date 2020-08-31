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

#include "readerThread.h"

//class MyWidget : public QWidget {
class MyWidget : public QMainWindow {   //主窗口我们选择从 QMainWindow 继承,这样可以有菜单栏 QMainWindow::menuBar() 可用

    Q_OBJECT

public:
    MyWidget(QWidget *parent = 0);

private slots:
    void onActOpen();
    void onActNew();
    void toggleStatusBar();

    //-----------------
    void onBtnOpen();
    void onBtnStartStop();
    void onSeek(int);

    void gotSigNum(int);

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
    QLineEdit *ledit_input;
    QPushButton *btn_open;
    QPushButton *btn_start_stop;
    QLabel *lbl_frame;
    QSlider *slider_progress;
    QLabel *lbl_progress;

    readerThread *reader;

protected:
    void keyPressEvent(QKeyEvent *e);   //处理按键事件
    void moveEvent(QMoveEvent *e);      //处理窗口移动事件
};
