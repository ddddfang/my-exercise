#与01类似，不同之处在于没有实现继承自wx.App的子类,而是直接使用这个类定义对象
#若系统只有一个窗口的话,可以这么做

import wx

if __name__=='__main__':
    app = wx.App()
    frame = wx.Frame(parent=None, title='Hello wxPython')
    frame.Show()
    app.MainLoop()
