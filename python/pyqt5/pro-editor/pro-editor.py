
#可否实现一个project解析,类似 source insight 将定义和引用parse出来
# http://ctags.sourceforge.net/ctags.html

# ctags -R --fields=+n+m+K zxnn_lib/app_c/zxnn_sample_c.h

# 一个交互式 ap
# id, func_name, 定义的地方(所在文件:所在行), 声明的地方(所在文件:所在行), 函数具体实现内容, 引用本函数的地方


# 一个结构体定义在何处,哪里引用了此结构体


# ctag 工作完成后,插入到db方便保存,工作时候需要读取db到dict,不然可能会慢
# 有没有必要使用sql,还是说只用 dict + pickle ? 

# 语法高亮
#   根据ctags检测出来的是 func/struct/...来, 还要有一个集合,eg. for if int 等语法关键字
#   需要高亮的包括: 系统内置关键词 (for if int define ...) + struct + func + enum + ...
#   #if 0 之间的code也要灰掉(编译器处理,编辑器只是显示区分)
#   qt自带一个实现语法高亮的class?? http://www.freesion.com/article/324778061/
# 左侧目录树
#   列出文件的函数和struct,单击跳转过去?,双击能选中?


# 特殊字符的处理
#   enter 换行自动缩进,显示行号怎么办? QtextEdit自带这个功能最好!
#   空格 单词(关键词)检查
#   () [] {} 匹配检查
#   . 和 -> 成员运算符,一直找到定义的地方,然后将成员展开

# 查找
#   文件中的查找和project范围的查找
#   类型: 普通字符串查找,函数引用定义查找,模糊查找,文件名查找(并打开)
# 跳转 link
#   跳转应该能跳回来,一个 stack / list ?
# 上下文
#   每输入一些单词,能匹配查找(select * where xx like xx)相似的目标, 在一个单独的窗口显示
#   能像 source insight 那样追踪再追踪



# https://doc.qt.io/archives/qtjambi-4.5.2_01/com/trolltech/qt/qtjambi-examples.html
# https://doc.qt.io/qt-5/qtwidgets-widgets-codeeditor-example.html
# https://zhuanlan.zhihu.com/p/34323103
# http://blog.chinaunix.net/uid-8210028-id-337877.html
# http://www.freesion.com/article/9939118651/
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
from PyQt5 import QtGui, QtCore
from PyQt5.QtCore import Qt

class Main(QtWidgets.QMainWindow): # 继承自系统类 QWidget
    def __init__(self):

        super().__init__()

        self.initUI()   # init 构造函数里调用了下面的 initUI()

    def initUI(self):

        self.model = QtWidgets.QFileSystemModel()
        self.model.setRootPath("./")
        self.treeview = QtWidgets.QTreeView()
        self.treeview.setModel(self.model)
        self.treeview.setAnimated(False)
        self.treeview.setSortingEnabled(True)
        self.treeview.setAlternatingRowColors(True)
        self.treeview.setColumnHidden(1, True)
        self.treeview.setColumnHidden(2, True)

        self.text = QtWidgets.QTextEdit(self)
        #self.text.setFont(QtGui.QFont("Menlo",12))
        me = QtGui.QFontMetrics(self.text.font())
        self.text.setTabStopWidth(4 * me.width(" "))   # 设置 tab 键宽度为 4 空格
        self.text.cursorPositionChanged.connect(self.cursorPosChanged)

        hbox = QtWidgets.QHBoxLayout()
        #hbox.addStretch(1)  # 就是加一个可伸缩的空格
        hbox.addWidget(self.treeview)
        hbox.addWidget(self.text)

        vbox = QtWidgets.QVBoxLayout()
        vbox.addLayout(hbox)

        self.cWidget = QtWidgets.QWidget()
        self.cWidget.setLayout(vbox)
        self.setCentralWidget(self.cWidget)


        self.initToolbar()
        self.initFormatbar()
        self.initMenuBar()
        self.statusbar = self.statusBar()

        #self.setGeometry(300, 300, 300, 220)        # 设置 显示的起始坐标 和 hw (相当于 resize() + move())
        self.resize(1030, 800)
        self.center()

        self.setWindowTitle('editor')
        self.setWindowIcon(QtGui.QIcon('icons/myedit.png')) # 我这里写的是相对路径,所以对运行时候的位置是有要求的,这个 icon 竟然跑到 ubuntu 的左边栏了...

    def initToolbar(self):
 
        self.newAction = QtWidgets.QAction(QtGui.QIcon("icons/new.png"),"New",self)
        self.newAction.setShortcut("Ctrl+N")
        self.newAction.setStatusTip("Create a new document from scratch.")
        self.newAction.triggered.connect(self.new)

        self.openAction = QtWidgets.QAction(QtGui.QIcon("icons/open.png"),"Open file",self)
        self.openAction.setStatusTip("Open existing document")
        self.openAction.setShortcut("Ctrl+O")
        self.openAction.triggered.connect(self.open)

        self.saveAction = QtWidgets.QAction(QtGui.QIcon("icons/save.png"),"Save",self)
        self.saveAction.setStatusTip("Save document")
        self.saveAction.setShortcut("Ctrl+S")
        self.saveAction.triggered.connect(self.save)

        self.findAction = QtWidgets.QAction(QtGui.QIcon("icons/find.png"),"Find and replace",self)
        self.findAction.setStatusTip("Find and replace words in your document")
        self.findAction.setShortcut("Ctrl+F")
        #self.findAction.triggered.connect(find.Find(self).show)    # 这是调用了另一个 find 类的方法,有点小复杂,先注释掉
        self.findAction.triggered.connect(self.find)

        self.cutAction = QtWidgets.QAction(QtGui.QIcon("icons/cut.png"),"Cut to clipboard",self)
        self.cutAction.setStatusTip("Delete and copy text to clipboard")
        self.cutAction.setShortcut("Ctrl+X")
        self.cutAction.triggered.connect(self.text.cut)

        self.copyAction = QtWidgets.QAction(QtGui.QIcon("icons/copy.png"),"Copy to clipboard",self)
        self.copyAction.setStatusTip("Copy text to clipboard")
        self.copyAction.setShortcut("Ctrl+C")
        self.copyAction.triggered.connect(self.text.copy)

        self.pasteAction = QtWidgets.QAction(QtGui.QIcon("icons/paste.png"),"Paste from clipboard",self)
        self.pasteAction.setStatusTip("Paste text from clipboard")
        self.pasteAction.setShortcut("Ctrl+V")
        self.pasteAction.triggered.connect(self.text.paste)

        self.undoAction = QtWidgets.QAction(QtGui.QIcon("icons/undo.png"),"Undo last action",self)
        self.undoAction.setStatusTip("Undo last action")
        self.undoAction.setShortcut("Ctrl+Z")
        self.undoAction.triggered.connect(self.text.undo)

        self.redoAction = QtWidgets.QAction(QtGui.QIcon("icons/redo.png"),"Redo last undone thing",self)
        self.redoAction.setStatusTip("Redo last undone thing")
        self.redoAction.setShortcut("Ctrl+Y")
        self.redoAction.triggered.connect(self.text.redo)

        self.toolbar = self.addToolBar("Options")
        self.toolbar.addAction(self.newAction)
        self.toolbar.addAction(self.openAction)
        self.toolbar.addAction(self.saveAction)
        self.toolbar.addSeparator()
        self.toolbar.addAction(self.cutAction)
        self.toolbar.addAction(self.copyAction)
        self.toolbar.addAction(self.pasteAction)
        self.toolbar.addAction(self.undoAction)
        self.toolbar.addAction(self.redoAction)
        self.toolbar.addSeparator()
        self.toolbar.addAction(self.findAction)
        self.addToolBarBreak()  # 让下一个 toolbar 出现在这个 toolbar 的下面

    def initFormatbar(self):
        fontBox = QtWidgets.QFontComboBox(self)
        fontBox.currentFontChanged.connect(lambda font: self.text.setCurrentFont(font))

        fontSize = QtWidgets.QSpinBox(self)
        fontSize.setSuffix(" pt")   # Will display " pt" after each value
        fontSize.valueChanged.connect(lambda size: self.text.setFontPointSize(size))

        fontColor = QtWidgets.QAction(QtGui.QIcon("icons/font-color.png"),"Change font color",self)
        fontColor.triggered.connect(self.fontColorChanged)

        backColor = QtWidgets.QAction(QtGui.QIcon("icons/highlight.png"),"Change background color",self)    # 所谓高亮就是改变背景颜色
        backColor.triggered.connect(self.highlight)

        boldAction = QtWidgets.QAction(QtGui.QIcon("icons/bold.png"),"Bold",self)
        boldAction.triggered.connect(self.bold)

        italicAction = QtWidgets.QAction(QtGui.QIcon("icons/italic.png"),"Italic",self)
        italicAction.triggered.connect(self.italic)

        underlAction = QtWidgets.QAction(QtGui.QIcon("icons/underline.png"),"Underline",self)
        underlAction.triggered.connect(self.underline)

        strikeAction = QtWidgets.QAction(QtGui.QIcon("icons/strike.png"),"Strike-out",self)
        strikeAction.triggered.connect(self.strike)

        alignLeft = QtWidgets.QAction(QtGui.QIcon("icons/align-left.png"),"Align left",self)
        alignLeft.triggered.connect(self.alignLeft)

        alignCenter = QtWidgets.QAction(QtGui.QIcon("icons/align-center.png"),"Align center",self)
        alignCenter.triggered.connect(self.alignCenter)

        alignRight = QtWidgets.QAction(QtGui.QIcon("icons/align-right.png"),"Align right",self)
        alignRight.triggered.connect(self.alignRight)

        alignJustify = QtWidgets.QAction(QtGui.QIcon("icons/align-justify.png"),"Align justify",self)
        alignJustify.triggered.connect(self.alignJustify)

        indentAction = QtWidgets.QAction(QtGui.QIcon("icons/indent.png"),"Indent Area",self)
        indentAction.setShortcut("Ctrl+Tab")
        indentAction.triggered.connect(self.indent)

        dedentAction = QtWidgets.QAction(QtGui.QIcon("icons/dedent.png"),"Dedent Area",self)
        dedentAction.setShortcut("Shift+Tab")
        dedentAction.triggered.connect(self.dedent)

        self.formatbar = self.addToolBar("Format")
        self.formatbar.addWidget(fontBox)
        self.formatbar.addWidget(fontSize)
        self.formatbar.addSeparator()

        self.formatbar.addAction(fontColor)
        self.formatbar.addAction(backColor)
        self.formatbar.addSeparator()

        self.formatbar.addAction(boldAction)
        self.formatbar.addAction(italicAction)
        self.formatbar.addAction(underlAction)
        self.formatbar.addAction(strikeAction)
        self.formatbar.addSeparator()

        self.formatbar.addAction(alignLeft)
        self.formatbar.addAction(alignCenter)
        self.formatbar.addAction(alignRight)
        self.formatbar.addAction(alignJustify)
        self.formatbar.addSeparator()

        self.formatbar.addAction(indentAction)
        self.formatbar.addAction(dedentAction)

    def initMenuBar(self):
        menubar = self.menuBar()
        mfile = menubar.addMenu("File")
        medit = menubar.addMenu("Edit")
        mview = menubar.addMenu("View")

        mfile.addAction(self.newAction)
        mfile.addAction(self.openAction)
        mfile.addAction(self.saveAction)

        medit.addAction(self.cutAction)
        medit.addAction(self.copyAction)
        medit.addAction(self.pasteAction)
        medit.addAction(self.undoAction)
        medit.addAction(self.redoAction)
        medit.addAction(self.findAction)

        # 控制是否显示 工具栏和状态栏, view 就是干这个的
        treeviewAction = QtWidgets.QAction("TreeView",self,checkable=True)
        treeviewAction.setChecked(True)
        treeviewAction.triggered.connect(self.toggleTreeView)

        toolbarAction = QtWidgets.QAction("Toolbar",self,checkable=True)
        toolbarAction.setChecked(True)
        toolbarAction.triggered.connect(self.toggleToolbar)

        formatbarAction = QtWidgets.QAction("Formatbar",self,checkable=True)
        formatbarAction.setChecked(True)
        formatbarAction.triggered.connect(self.toggleFormatbar)

        statusbarAction = QtWidgets.QAction("Statusbar",self,checkable=True)
        statusbarAction.setChecked(True)
        statusbarAction.triggered.connect(self.toggleStatusbar)

        mview.addAction(toolbarAction)
        mview.addAction(formatbarAction)
        mview.addAction(statusbarAction)
        mview.addAction(treeviewAction)

    def center(self):
        qr = self.frameGeometry()   # get a rectangle specifying the geometry of the main window, includes any window frame
        cp = QtWidgets.QDesktopWidget().availableGeometry().center()  # get the center point.
        qr.moveCenter(cp)           # set the center of the rectangle to the center of the screen. The rectangle's size is unchanged.
        self.move(qr.topLeft())

    def new(self):
        print("in new func, not implement yet.")

    def open(self):

        # Get filename and show only .writer files
        # PYQT5 Returns a tuple in PyQt5, we only need the filename
        self.filename = QtWidgets.QFileDialog.getOpenFileName(self, 'Open File',".","(*.*)")[0]

        if self.filename:
            with open(self.filename,"rt") as file:
                self.text.setText(file.read())

    def save(self):
        print("in save func, not implement yet.")

    def find(self):
        print("in find func, not implement yet.")

    def bold(self):
        if self.text.fontWeight() == QtGui.QFont.Bold:
            self.text.setFontWeight(QtGui.QFont.Normal)
        else:
            self.text.setFontWeight(QtGui.QFont.Bold)

    def italic(self):
        state = self.text.fontItalic()
        self.text.setFontItalic(not state)

    def underline(self):
        state = self.text.fontUnderline()
        self.text.setFontUnderline(not state)

    def strike(self):   # 加删除线
        # Grab the text's format
        fmt = self.text.currentCharFormat()
        # Set the fontStrikeOut property to its opposite
        fmt.setFontStrikeOut(not fmt.fontStrikeOut())
        # And set the next char format
        self.text.setCurrentCharFormat(fmt)

    def alignLeft(self):
        self.text.setAlignment(Qt.AlignLeft)

    def alignRight(self):
        self.text.setAlignment(Qt.AlignRight)

    def alignCenter(self):
        self.text.setAlignment(Qt.AlignCenter)

    def alignJustify(self):
        self.text.setAlignment(Qt.AlignJustify)
    
    def indent(self):
        print("in indent func, not implement yet.")

    def dedent(self):
        print("in dedent func, not implement yet.")

    def highlight(self):
        print("in highlight func, not implement yet.")

    def cursorPosChanged(self):
        cur = self.text.textCursor()
        showMsg = "line {} col {}".format(cur.blockNumber(), cur.columnNumber())

        if cur.hasSelection():
            prevCur = QtGui.QTextCursor(cur.currentFrame())
            prevCur.setPosition(cur.anchor())
            selectedLines = abs(prevCur.blockNumber() - cur.blockNumber()) + 1  # 计算出选中的行数
            selectedChars = cur.selectionEnd() - cur.selectionStart()           # 计算出选中的字符数
            showMsg = "".join([showMsg, ", {} lines & {} chars selected.".format(selectedLines, selectedChars)]) #这是选中了多少个char,而不是多少行
        self.statusbar.showMessage(showMsg)

    #def toggleMenu(self, state):
    #    if state:
    #        self.statusbar.show()
    #    else:
    #        self.statusbar.hide()

    def toggleTreeView(self):
        state = self.treeview.isVisible()
        self.treeview.setVisible(not state)

    def toggleToolbar(self):
        state = self.toolbar.isVisible()
        self.toolbar.setVisible(not state)

    def toggleFormatbar(self):
        state = self.formatbar.isVisible()
        self.formatbar.setVisible(not state)

    def toggleStatusbar(self):
        state = self.statusbar.isVisible()
        self.statusbar.setVisible(not state)

    def fontColorChanged(self):
        # Get a color from the text dialog
        color = QtWidgets.QColorDialog.getColor()
        # Set it as the new text color
        self.text.setTextColor(color)

    # override QtWidgets.QWidget.closeEvent()
    def closeEvent(self, event):
        reply = QtWidgets.QMessageBox.question(self, 'Message', "Are you sure to quit?",
                                               QtWidgets.QMessageBox.Yes | QtWidgets.QMessageBox.No,
                                               QtWidgets.QMessageBox.No)
        if reply == QtWidgets.QMessageBox.Yes:
            event.accept()
        else:
            event.ignore()

def main():
    app = QtWidgets.QApplication(sys.argv)
    editorMain = Main()
    editorMain.show()
    sys.exit(app.exec_())   # exec is a Python keyword. And thus, exec_() was used instead

if __name__ == '__main__':
    main()
