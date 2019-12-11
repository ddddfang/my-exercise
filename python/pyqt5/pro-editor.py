
# 最官方的文档当然是这个
# https://wiki.python.org/moin/PyQt

# 这个人写了一个编辑器,使用pyqt
#第 1部分：http://www.binpress.com/tutorial/building-a-text-editor-with-pyqt-part-one/143
#第 2部分：http://www.binpress.com/tutorial/developing-a-pyqt-text-editor-part-2/145
#第 3部分：http://www.binpress.com/tutorial/building-a-text-editor-with-pyqt-part-3/147
#第 4部分：http://www.binpress.com/tutorial/building-a-text-editor-with-pyqt-part-4/148

import sys

from PyQt5 import QtWidgets
from PyQt5 import QtGui


class Main(QtWidgets.QWidget): # 继承自系统类 QWidget
    def __init__(self):
        super().__init__()
        self.initUI()   # init 构造函数里调用了下面的 initUI()

    def initUI(self):
        QtWidgets.QToolTip.setFont(QtGui.QFont('SansSerif', 10))    # 这应该是一个静态的方法,10pt SansSerif font.
        self.setToolTip('This is a <b>QWidget</b> widget')

        btn = QtWidgets.QPushButton('Button', self)   # QPushButton(string text, QWidget parent = None),参数1表示显示的文字,参数2表示button放置的 QWidget
        # 注册时间处理函数,可以是任何 Qt slot 或 python可调用对象
        btn.clicked.connect(QtWidgets.QApplication.instance().quit)

        btn.setToolTip('This is a <b>QPushButton</b> widget')
        btn.resize(btn.sizeHint())  # sizeHint() 返回一个推荐大小
        btn.move(50, 50)            # 这个是相对于 window 的pos

        #self.setGeometry(300, 300, 300, 220)        # 设置 显示的起始坐标 和 hw (相当于 resize() + move())
        self.resize(300, 220)
        self.center()

        self.setWindowTitle('demo02')
        self.setWindowIcon(QtGui.QIcon('res/simple.png')) # 我这里写的是相对路径,所以对运行时候的位置是有要求的,这个 icon 竟然跑到 ubuntu 的左边栏了...
        self.show()

    # override QtWidgets.QWidget.closeEvent()
    def closeEvent(self, event):
        reply = QtWidgets.QMessageBox.question( self, 
                                                'Message',
                                                "Are you sure to quit?",
                                                QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No,
                                                QtWidgets.QMessageBox.No)
        if reply == QtWidgets.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

    def center(self):
        qr = self.frameGeometry()   # get a rectangle specifying the geometry of the main window, includes any window frame
        cp = QtWidgets.QDesktopWidget().availableGeometry().center()  # get the center point.
        qr.moveCenter(cp)           # set the center of the rectangle to the center of the screen. The rectangle's size is unchanged.
        self.move(qr.topLeft())

def main():
    app = QtWidgets.QApplication(sys.argv)
    editor = Main()
    sys.exit(app.exec_())   # exec is a Python keyword. And thus, exec_() was used instead

if __name__ == '__main__':
    main()
