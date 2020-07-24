//#include <QtCore>
//#include <iostream>
//
//int main() {
//
//    std::cout << "Qt version: " << qVersion() << std::endl;
//}


#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    QWidget window;

    window.resize(250, 150);
    window.setWindowTitle("Simple example");
    window.show();

    return app.exec();
}
