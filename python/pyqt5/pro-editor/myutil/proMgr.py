import os
from PyQt5 import QtWidgets
from PyQt5 import QtGui, QtCore
from PyQt5.QtCore import Qt

class proMgr():
    
    def __init__(self, parent = None):

        self.parent = parent
        self.filetypes = [".py",".c",".h",".cc",".cpp",".hpp"]

    def proNew(self):
        
    def openPath(self):
        pass
    def addFiles(filenames):
        pass
    def removeFiles():
        pass
    def sync():
        pass
    def getFiles(path):
        result = []
        for curdir, dirname, filename in os.walk(path):
            for f in filename:
                if os.path.splitext(f)[1] in filefilter:
                    fullname = os.path.join(curdir, f)
                    result.append(fullname)
        return result
