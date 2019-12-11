import sys
from PyQt5.QtWidgets import (QWidget, QToolTip, QPushButton, QApplication, QMessageBox, QDesktopWidget, QMainWindow, QAction, qApp)
from PyQt5.QtGui import QIcon
from PyQt5.QtGui import QFont

class example(QMainWindow): 
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        QToolTip.setFont(QFont('SansSerif', 10))
        self.setToolTip('This is a <b>QMainWindow</b> widget')

        exitAct = QAction(QIcon('icons/exit.png'), '&Exit', self)   # 单纯的定义了一个动作
        exitAct.setShortcut('Ctrl+Q')
        exitAct.setStatusTip('Exit application')
        exitAct.triggered.connect(qApp.quit)

        btn = QPushButton('Button', self)
        btn.clicked.connect(QApplication.instance().quit)
        btn.setToolTip('This is a <b>QPushButton</b> widget')
        btn.resize(btn.sizeHint())
        btn.move(50, 50)

        #self.statusBar().showMessage('Ready')
        self.statusBar()

        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File') # 添加一个菜单
        fileMenu.addAction(exitAct)         # 添加一个动作


        #self.setGeometry(300, 300, 300, 220)
        self.resize(300, 220)
        self.center()

        self.setWindowTitle('demo02')
        self.setWindowIcon(QIcon('res/simple.png'))
        self.show()

    # closeEvent() 是 QWidget 定义的函数,这里我们重新实现它
    def closeEvent(self, event):
        reply = QMessageBox.question(self, 'Message', "Are you sure to quit?", QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

        if reply == QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

    def center(self):

        qr = self.frameGeometry()   # get a rectangle specifying the geometry of the main window, includes any window frame
        cp = QDesktopWidget().availableGeometry().center()  # get the center point.
        qr.moveCenter(cp)           # set the center of the rectangle to the center of the screen. The rectangle's size is unchanged.
        self.move(qr.topLeft())


if __name__ == '__main__':

    app = QApplication(sys.argv)
    exw = example()
    sys.exit(app.exec_())   # exec is a Python keyword. And thus, exec_() was used instead
