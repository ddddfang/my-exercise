
import wx

class MyFrame(wx.Frame):
    def __init__(self, parent, id):
        # parent 为 None 表示此窗口是顶级窗口,id=-1表示让系统自动生成id号
        # 还可以给出 name 表示框架内在名字,用它来寻找这个窗口
        wx.Frame.__init__(self, parent, id, title="hello wxpython demo.", pos=(100,100), size=(600,400))
        panel = wx.Panel(self)  #创建画板

        #title = wx.StaticText(panel, label="this is title", pos=(100,20))
        #font = wx.Font(16, wx.DEFAULT, wx.FONTSTYLE_NORMAL, wx.NORMAL)
        #title.SetFont(font)
        #wx.StaticText(panel, label="this is line 1.", pos=(50,50))
        #wx.StaticText(panel, label="this is line 2.", pos=(50,70))
        #wx.StaticText(panel, label="this is line 2.", pos=(50,90))

        self.title = wx.StaticText(panel, label="please enter user name and passwd", pos=(140, 120))
        self.label_user = wx.StaticText(panel, label="user name:", pos=(50,150))
        self.text_user = wx.TextCtrl(panel, pos=(200, 150), size=(235,25), style=wx.TE_LEFT)
        self.label_passwd = wx.StaticText(panel, label="user passwd:", pos=(50,190))
        self.text_passwd = wx.TextCtrl(panel, pos=(200, 190), size=(235,25), style=wx.TE_PASSWORD)

        print("self.text_user:{},self.text_passwd:{}".format(self.text_user, self.text_passwd))

        self.bt_confirm = wx.Button(panel, label="OK", pos=(50, 230))
        self.bt_cancel = wx.Button(panel, label="Cancel", pos=(150, 230))

if __name__=='__main__':
    app = wx.App()
    frame = MyFrame(parent=None, id=-1)
    frame.Show()
    app.MainLoop()
