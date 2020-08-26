//#include <QApplication>
//#include <QWidget>
//#include <QIcon>
//#include <QFrame>
//#include <QGridLayout>
//#include <QPushButton>
#include "myWidget.h"


//把他们放到单独的 MyWidget.h.cpp 中岂不美哉?
//class MyWidget : public QWidget {
//public:
//    MyWidget(QWidget *parent = 0);
//};
//
////MyWidget::MyWidget(QWidget *parent) : QWidget(parent) {
////    QFrame *frame1 = new QFrame(this);
////    frame1->setFrameStyle(QFrame::Box);
////    frame1->setCursor(Qt::SizeAllCursor);
////
////    QFrame *frame2 = new QFrame(this);
////    frame2->setFrameStyle(QFrame::Box);
////    frame2->setCursor(Qt::WaitCursor);
////
////    QFrame *frame3 = new QFrame(this);
////    frame3->setFrameStyle(QFrame::Box);
////    frame3->setCursor(Qt::PointingHandCursor);
////
////    QGridLayout *grid = new QGridLayout(this);
////    grid->addWidget(frame1, 0, 0);
////    grid->addWidget(frame2, 0, 1);
////    grid->addWidget(frame3, 0, 2);
////    setLayout(grid);
////}
//
//MyWidget::MyWidget(QWidget *parent) : QWidget(parent) {
//    QPushButton *btnQuit = new QPushButton("Quit", this);
//    btnQuit->setGeometry(50, 40, 75, 30);
//
//    //qApp 是一个全局指针,指向 QApplication 对象,在 QApplication 头文件中定义
//    connect(btnQuit, &QPushButton::clicked, qApp, &QApplication::quit);
//}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   //qt5 的非console程序,都必须要创建 QApplication 对象

    //QWidget window;
    MyWidget window;    //使用自己定制的 QWidget,所以这样定制的 QWidget 可以被放到单独的模块里

    window.resize(250, 150);
    window.move(300, 300);
    window.setWindowTitle("simple sample");
    window.setWindowIcon(QIcon("icon.png"));    //app启动后左侧dock栏会显示这个图标
    //window.setToolTip("QWidget");   //当鼠标在主窗口停留的时候会显示这个提示
    window.show();

    return app.exec();
}
