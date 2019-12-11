import sys
from PyQt5.QtWidgets import (QWidget, QToolTip, QPushButton, QApplication, QMessageBox, QDesktopWidget, QMainWindow, QAction, qApp, QMenu)
from PyQt5.QtGui import QIcon
from PyQt5.QtGui import QFont

class example(QMainWindow): 
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.statusbar = self.statusBar()
        self.statusbar.showMessage('Ready')

        actImp = QAction('Import mail', self) 
        actNew = QAction('New', self)

        menuImp = QMenu('Import', self)     # 创建菜单
        menuImp.addAction(actImp)           # 为菜单添加其下的操作

        menubar = self.menuBar()    # 创建菜单栏
        fileMenu = menubar.addMenu('File')  # 创建一级菜单
        fileMenu.addAction(actNew)          # 为一级菜单添加其下的操作
        fileMenu.addMenu(menuImp)           # 为一级菜单添加其下的子菜单
        editMenu = menubar.addMenu('Edit')  # 创建一级菜单

        btn = QPushButton('Button', self)
        #btn.clicked.connect(QApplication.instance().quit)
        btn.clicked.connect(self.say_hello)
        btn.resize(btn.sizeHint())
        btn.move(50, 50)


        #self.setGeometry(300, 300, 300, 220)
        self.resize(300, 220)
        self.center()

        self.setWindowTitle('demo03')
        self.setWindowIcon(QIcon('res/simple.png'))
        self.show()

    # closeEvent() 是 QWidget 定义的函数,这里我们重新实现它
    def closeEvent(self, event):
        reply = QMessageBox.question(self, 'Message', "Are you sure to quit?", QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

        if reply == QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

    def say_hello(self):
        print ("hello")

    def center(self):

        qr = self.frameGeometry()   # get a rectangle specifying the geometry of the main window, includes any window frame
        cp = QDesktopWidget().availableGeometry().center()  # get the center point.
        qr.moveCenter(cp)           # set the center of the rectangle to the center of the screen. The rectangle's size is unchanged.
        self.move(qr.topLeft())


if __name__ == '__main__':

    app = QApplication(sys.argv)
    exw = example()
    sys.exit(app.exec_())   # exec is a Python keyword. And thus, exec_() was used instead
