import sys
from PyQt5.QtWidgets import (QWidget, QToolTip, QPushButton, QApplication, QMessageBox, QDesktopWidget, QMainWindow, QAction, qApp, QMenu, QTextEdit)
from PyQt5.QtGui import QIcon
from PyQt5.QtGui import QFont

class example(QMainWindow): 
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):

        # actions
        actImp = QAction('Import mail', self) 
        actNew = QAction('New', self)
        actExit = QAction(QIcon('res/exit.png'), 'Exit', self)
        actExit.setShortcut('Ctrl+Q')
        actExit.triggered.connect(qApp.quit)

        actViewStat = QAction('View statusbar', self, checkable=True)
        actViewStat.setChecked(True)
        actViewStat.setStatusTip('View statusbar')
        actViewStat.triggered.connect(self.toggleMenu)

        # sub menus
        menuImp = QMenu('Import', self)     # 创建菜单
        menuImp.addAction(actImp)           # 为菜单添加其下的操作

        #################### 菜单栏 #########
        menubar = self.menuBar()    # 创建菜单栏
        ####
        fileMenu = menubar.addMenu('File')  # 创建一级菜单
        fileMenu.addAction(actNew)          # 为一级菜单添加其下的操作
        fileMenu.addMenu(menuImp)           # 为一级菜单添加其下的子菜单
        ####
        viewMenu = menubar.addMenu('View')  # 创建一级菜单
        viewMenu.addAction(actViewStat)

        #################### 工具栏 #########
        self.toolbar = self.addToolBar('Exit')
        self.toolbar.addAction(actExit)

        #################### 状态栏 #########
        self.statusbar = self.statusBar()
        #self.statusbar.showMessage('Ready')

        #btn = QPushButton('Button', self)
        ##btn.clicked.connect(QApplication.instance().quit)
        #btn.clicked.connect(self.say_hello)
        #btn.resize(btn.sizeHint())
        #btn.move(50, 50)

        textEdit = QTextEdit()
        self.setCentralWidget(textEdit) # central widget 会占据除了菜单栏,工具栏,状态栏之外的所有空间


        #self.setGeometry(300, 300, 300, 220)
        self.resize(300, 220)
        self.center()

        self.setWindowTitle('demo03')
        self.setWindowIcon(QIcon('res/simple.png'))
        self.show()

    # override closeEvent()
    def closeEvent(self, event):
        reply = QMessageBox.question(self, 'Message', "Are you sure to quit?", QMessageBox.Yes | QMessageBox.No, QMessageBox.No)

        if reply == QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

    # override contextMenuEvent() # 右键的时候会弹出 contextMenu
    def contextMenuEvent(self, event):
        cmenu = QMenu(self)

        actNew = cmenu.addAction("New")
        actOpen = cmenu.addAction("Open")
        actQuit = cmenu.addAction("Quit")

        action = cmenu.exec_(self.mapToGlobal(event.pos()))
        if action == actQuit:
            print("ctx menu: actQuit.")
            #qApp.quit()
        elif action == actNew:
            print("ctx menu: actNew.")
        elif action == actOpen:
            print("ctx menu: actOpen.")

    def say_hello(self):
        print ("hello")

    def toggleMenu(self, state):
        if state:
            self.statusbar.show()
        else:
            self.statusbar.hide()

    def center(self):
        qr = self.frameGeometry()   # get a rectangle specifying the geometry of the main window, includes any window frame
        cp = QDesktopWidget().availableGeometry().center()  # get the center point.
        qr.moveCenter(cp)           # set the center of the rectangle to the center of the screen. The rectangle's size is unchanged.
        self.move(qr.topLeft())


if __name__ == '__main__':

    app = QApplication(sys.argv)
    exw = example()
    sys.exit(app.exec_())   # exec is a Python keyword. And thus, exec_() was used instead
