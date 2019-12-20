# https://blog.csdn.net/u013250071/article/details/79037843

import smtplib
from email.mime.multipart import MIMEMultipart
from email.mime.text import MIMEText
from email.mime.image import MIMEImage
from email.header import Header
from email.mime.base import MIMEBase
from email import encoders

# 第三方 SMTP 服务
mail_host = "smtp.163.com"        #设置服务器
mail_user = "17621308623@163.com" #用户名
mail_pass = "fang716617"          #口令


sender = mail_user
receivers = ['1027082315@qq.com','FridayFang@zhaoxin.com']  #

message = MIMEMultipart()
message['From'] = sender    #这是给message填充的字段,不会对实际发送到哪里或由谁发送起决定作用,但是会被sendmail校验,可能会导致发送失败
message['To'] = ",".join(receivers) #这里貌似不会被校验,但是会显示在收件人一栏.所以最好和上面的receivers[i]相同
message['Subject'] = Header('这里是主题', 'utf-8')


message.attach(MIMEText('来来来，这是邮件的正文', 'plain', 'utf-8'))
att1 = MIMEText(open('text1.txt', 'rb').read(), 'base64', 'utf-8')
att1["Content-Type"] = 'application/octet-stream'
att1["Content-Disposition"] = 'attachment; filename="text1.txt"'
message.attach(att1)

att2 = MIMEText(open('123.jpg', 'rb').read(), 'base64', 'utf-8')
att2["Content-Type"] = 'application/octet-stream'
att2["Content-Disposition"] = 'attachment; filename="123.jpg"'
message.attach(att2)

## 构造附件3（附件为HTML格式的网页）
#att3 = MIMEText(open('report_test.html', 'rb').read(), 'base64', 'utf-8')
#att3["Content-Type"] = 'application/octet-stream'
#att3["Content-Disposition"] = 'attachment; filename="report_test.html"'
#message.attach(att3)

try:
    s = smtplib.SMTP_SSL(mail_host, 465)   #不带ssl也可以 s = smtplib.SMTP(mail_host, 25)
    s.login(mail_user, mail_pass)
    s.sendmail(sender, receivers[0], message.as_string())  #接收者可以是一个也可以是列表
    #s.sendmail(sender, receivers, message.as_string())
    print ("邮件发送成功")
    s.quit()
except smtplib.SMTPException:
    print ("Error: 无法发送邮件")
