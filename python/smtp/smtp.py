
import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.image import MIMEImage
from email.header import Header

# 第三方 SMTP 服务
mail_host = "smtp.163.com"        #设置服务器
mail_user = "17621308623@163.com" #用户名
mail_pass = "fang716617"          #口令


sender = mail_user
receivers = ['1027082315@qq.com','FridayFang@zhaoxin.com']  #

#创建一封普通邮件(不带附件)
#message = MIMEText('这是一封来自系统的邮件,请勿回复...', 'plain', 'utf-8')
content = '''   <html>
                <body>
                    <h1>Hello</h1>
                    <p>send by <a href="http://www.python.org">Python</a>...</p>
                </body></html>'''
message = MIMEText(content, 'html', 'utf-8')




message['From'] = sender    #这是给message填充的字段,不会对实际发送到哪里或由谁发送起决定作用,但是会被sendmail校验,可能会导致发送失败
message['To'] = ",".join(receivers) #这里貌似不会被校验,但是会显示在收件人一栏.所以最好和上面的receivers[i]相同
message['Subject'] = Header('这里是主题', 'utf-8')


try:
    s = smtplib.SMTP_SSL(mail_host, 465)   #不带ssl也可以 s = smtplib.SMTP(mail_host, 25)
    s.login(mail_user, mail_pass)
    #s.sendmail(sender, receivers[0], message.as_string())  #接收者可以是一个也可以是列表
    s.sendmail(sender, receivers, message.as_string())
    print ("邮件发送成功")
except smtplib.SMTPException:
    print ("Error: 无法发送邮件")
