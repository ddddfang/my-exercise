//#include <QApplication>
//#include <QWidget>
//#include <QIcon>
//#include <QFrame>
//#include <QGridLayout>
//#include <QPushButton>
#include "myWidget.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   //qt5 的非console程序,都必须要创建 QApplication 对象

    MyWidget window;    //使用自己定制的 QWidget,所以这样定制的 QWidget 可以被放到单独的模块里

    window.resize(800, 400);
    window.move(300, 300);
    window.setWindowTitle("simple sample");
    window.setWindowIcon(QIcon("icon.png"));    //app启动后左侧dock栏会显示这个图标
    window.show();

    return app.exec();
}
