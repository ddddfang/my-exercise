# http://zetcode.com/gui/pyqt5/firstprograms/


import sys
# QDesktopWidget 提供了关于用户桌面的一些信息,包括 screen size
from PyQt5.QtWidgets import (QWidget, QToolTip, QPushButton, QApplication, QMessageBox, QDesktopWidget)
from PyQt5.QtGui import QIcon
from PyQt5.QtGui import QFont

class example(QWidget): # 继承自系统类 QWidget
    def __init__(self):
        super().__init__()
        self.initUI()   # init 构造函数里调用了下面的 initUI()

    def initUI(self):
        QToolTip.setFont(QFont('SansSerif', 10))    # 这应该是一个静态的方法,10pt SansSerif font.
        self.setToolTip('This is a <b>QWidget</b> widget')

        btn = QPushButton('Button', self)   # QPushButton(string text, QWidget parent = None),参数1表示显示的文字,参数2表示button放置的 QWidget
        # 注册时间处理函数,可以是任何 Qt slot 或 python可调用对象
        btn.clicked.connect(QApplication.instance().quit)

        btn.setToolTip('This is a <b>QPushButton</b> widget')
        btn.resize(btn.sizeHint())  # sizeHint() 返回一个推荐大小
        btn.move(50, 50)            # 这个是相对于 window 的pos

        #self.setGeometry(300, 300, 300, 220)        # 设置 显示的起始坐标 和 hw (相当于 resize() + move())
        self.resize(300, 220)
        self.center()

        self.setWindowTitle('demo02')
        self.setWindowIcon(QIcon('res/simple.png')) # 我这里写的是相对路径,所以对运行时候的位置是有要求的,这个 icon 竟然跑到 ubuntu 的左边栏了...
        self.show()

    # closeEvent() 是 QWidget 定义的函数,这里我们重新实现它(注意是点击x才会触发,QApplication.instance().quit()不会触发此函数)
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
