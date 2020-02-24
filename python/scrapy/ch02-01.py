
import requests
from bs4 import BeautifulSoup



#------------------------------------------------------------
link = "http://www.santostang.com/"

# 定义请求头的浏览器代理,伪装成浏览器,可以在开发者工具->网络,查看xhr(异步请求的数据)信息,
# 点击一个数据包后,消息头字段可看到
headers = {'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:65.0) Gecko/20100101 Firefox/65.0'}

# 获得服务 response 对象
r = requests.get(url = link, headers = headers)

print(r.encoding, r.status_code)
print(r.url)
#print(r.text)   # r.text 是网页的源码 html文档
with open("./text.html", "a") as fw:
    fw.write(r.content.decode(r.encoding))


#------------------------------------------------------------
soup = BeautifulSoup(r.text, "html.parser") # 使用 BeautifulSoup 解析 html, 得到 soup 对象
# 找到第一篇文章的标题. 定位到 class是 "post-title" 的 h1 元素元素 --> 提取a --> 提取其中的字符串并去掉左右的空格
# 这个元素的通用查找方法是通过浏览器的 查看元素 选项,选中元素会自动定位到对应的html代码
title = soup.find("h1", class_ = "post-title").a.text.strip()
#print(title)
with open("./title.txt", "a+") as fw:
    fw.write(title)
