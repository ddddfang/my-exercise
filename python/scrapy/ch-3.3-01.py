
import requests
from bs4 import BeautifulSoup


#------------------------------------------------------------
link = 'http://httpbin.org/get'

# get 请求的参数 requests 帮助我们处理,会写在 url 里
key_dict = {'key1': 'value1', 'key2': 'value2'}

# 定义请求头的浏览器代理,伪装成浏览器
headers = {'User-Agent': 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:65.0) Gecko/20100101 Firefox/65.0'}
r = requests.get(url=link, params=key_dict, headers=headers, timeout=20)    #超时时间为20s,超时会抛出异常!

print(r.encoding, r.status_code)
print(r.url)
print(r.text)   # r.text 是网页的源码 html文档
#with open("./text.html", "a") as fw:
#    fw.write(r.content.decode(r.encoding))


link = 'http://httpbin.org/post'
# post请求的参数会放在数据包的body中,requests可以帮忙处理
r = requests.post(url=link, data=key_dict)
print(r.text)
