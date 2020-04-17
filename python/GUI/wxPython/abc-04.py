
import wx

class MyFrame(wx.Frame):
    def __init__(self, parent, id):
        # parent 为 None 表示此窗口是顶级窗口,id=-1表示让系统自动生成id号
        # 还可以给出 name 表示框架内在名字,用它来寻找这个窗口
        wx.Frame.__init__(self, parent, id, title="hello wxpython demo.", size=(400,300))
        panel = wx.Panel(self)  #创建画板

        self.title = wx.StaticText(panel, label="please enter user name and passwd")

        self.label_user = wx.StaticText(panel, label="user name:", size=(100,20))
        self.text_user = wx.TextCtrl(panel, style=wx.TE_LEFT)   #一段左对齐的普通文本
        self.label_passwd = wx.StaticText(panel, label="user passwd:", size=(100,20))
        self.text_passwd = wx.TextCtrl(panel, style=wx.TE_PASSWORD)

        self.bt_confirm = wx.Button(panel, label="OK", pos=(50, 230))
        self.bt_cancel = wx.Button(panel, label="Cancel", pos=(150, 230))
        self.bt_confirm.Bind(wx.EVT_BUTTON, self.onClickOK)
        self.bt_cancel.Bind(wx.EVT_BUTTON, self.onClickCancel)

        hsizer_user = wx.BoxSizer(wx.HORIZONTAL)   #布局器
        hsizer_user.Add(self.label_user, proportion=0, flag=wx.ALL, border=5)   #0表示占据1份,且不随着size调整
        hsizer_user.Add(self.text_user, proportion=1, flag=wx.ALL, border=5)
        hsizer_pwd = wx.BoxSizer(wx.HORIZONTAL)   #布局器
        hsizer_pwd.Add(self.label_passwd, proportion=0, flag=wx.ALL, border=5)
        hsizer_pwd.Add(self.text_passwd, proportion=1, flag=wx.ALL, border=5)
        hsizer_button = wx.BoxSizer(wx.HORIZONTAL)   #布局器
        hsizer_button.Add(self.bt_confirm, proportion=0, flag=wx.ALIGN_CENTER, border=5)
        hsizer_button.Add(self.bt_cancel, proportion=0, flag=wx.ALIGN_CENTER, border=5)

        vsizer_all = wx.BoxSizer(wx.VERTICAL)   #布局器,纵向排列
        vsizer_all.Add(self.title, proportion=0, flag=wx.BOTTOM|wx.TOP|wx.ALIGN_CENTER, border=15)
        vsizer_all.Add(hsizer_user, proportion=0, flag=wx.EXPAND|wx.LEFT|wx.RIGHT, border=45)
        vsizer_all.Add(hsizer_pwd, proportion=0, flag=wx.EXPAND|wx.LEFT|wx.RIGHT, border=45)
        vsizer_all.Add(hsizer_button, proportion=0, flag=wx.TOP|wx.ALIGN_CENTER, border=15)
        panel.SetSizer(vsizer_all)

    def onClickOK(self, event):
        message = ''
        user_name = self.text_user.GetValue()
        user_passwd = self.text_passwd.GetValue()
        print("ok.{},{}".format(user_name, user_passwd))
        if user_name=='' or user_passwd=='':
            message = 'user name or passwd cannot be empty'
        elif user_name=='fang' and user_passwd==' ':
            message = 'login success'
        else:
            message = 'login error'
        wx.MessageBox(message)


    def onClickCancel(self, event):
        print("cancel..")
        self.text_user.SetValue('')
        self.text_passwd.SetValue('')

if __name__=='__main__':
    app = wx.App()
    frame = MyFrame(parent=None, id=-1)
    frame.Show()
    app.MainLoop()
