//#include <QApplication>
//#include <QWidget>
//#include <QIcon>
//#include <QFrame>
//#include <QGridLayout>
//#include <QPushButton>
#include "mainWindow.h"
#include "apps/live/liveWidget.h"

//#include <opencv2/opencv.hpp>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   //qt5 的非console程序,都必须要创建 QApplication 对象

    MainWidget window;    //使用自己定制的 QWidget,所以这样定制的 QWidget 可以被放到单独的模块里
    //LiveWidget window;    //这里直接使用 LiveWidget 也是可以的,很方便 集成 和 单独测试

    window.resize(800, 600);
    window.move(300, 300);
    window.setWindowTitle("simple sample");
    window.setWindowIcon(QIcon("./res/icon.png"));    //app启动后左侧dock栏会显示这个图标
    window.show();

    ////这样这里会显示两个窗口,一个我们的主窗口,一个cv的窗口
    //cv::Mat image = cv::imread("/home/fang/testvideo/x264.jpg");
    //cv::imshow("cvOutPut", image);  //使用qt的显示框架显示这个图片(使用qt/gtk要看cmake指定的是什么)

    return app.exec();
}
