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


//class MyWidget : public QWidget {
class MyWidget : public QMainWindow {   //主窗口我们选择从 QMainWindow 继承,这样可以有菜单栏 QMainWindow::menuBar() 可用

    Q_OBJECT

public:
    MyWidget(QWidget *parent = 0);

private slots:
    void onActOpen();
    void onActNew();
    void toggleStatusBar();

    void onBtnYes();
    void onBtnNo();
    void onCheckBoxStateChange();

    void onBtnAddLw();
    void onBtnRemoveLw();
    void onBtnRenameLw();
    void onBtnClearLw();

    void onTimerPbar();
    void onBtnStart();
    void onBtnStop();
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
    QPushButton *btn_yes;
    QPushButton *btn_no;
    QLabel *lbl_time;

    QLineEdit *ledit_name;
    QLineEdit *ledit_phone;
    QLineEdit *ledit_addr;

    QCheckBox *checkb;
    QLabel *lbl_for_check;

    QListWidget *lw;
    QPushButton *btn_add_for_lw;
    QPushButton *btn_remove_for_lw;
    QPushButton *btn_rename_for_lw;
    QPushButton *btn_clear_for_lw;

    QProgressBar *progress_bar;
    QTimer *timer_for_pbar;
    QPushButton *btn_start_for_pbar;
    QPushButton *btn_stop_for_pbar;
    int progress;

protected:
    void keyPressEvent(QKeyEvent *e);   //处理按键事件
    void moveEvent(QMoveEvent *e);      //处理窗口移动事件
    void timerEvent(QTimerEvent *e);    //处理定时器事件
};
