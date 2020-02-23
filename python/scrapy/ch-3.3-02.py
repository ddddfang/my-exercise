# 爬取静态网页-->豆瓣top250电影
import requests
from bs4 import BeautifulSoup


#------------------------------------------------------------
# 定义请求头的浏览器代理,伪装成浏览器
headers = {
    'User-Agent' : 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:65.0) Gecko/20100101 Firefox/65.0',
    'Host' : 'movie.douban.com'
}

movie_list = []
for i in range(0,10):
    # 手动处理 get 请求的 param
    link = 'https://movie.douban.com/top250?start=' + str(i*25) + '&filter='
    r = requests.get(url=link, headers=headers, timeout=10)
    
    print("{} 页响应码 {}".format(i, r.status_code))
    #print(r.text)   # r.text 是网页的源码 html文档

    #soup = BeautifulSoup(r.text, "html.parser")
    #title = soup.find("h1", class_="post-title").a.text.strip()
    soup = BeautifulSoup(r.text, "lxml")
    div_list = soup.find_all("div", class_="hd")
    for each in div_list:
        movie = each.a.span.text.strip()
        movie_list.append(movie)

print(movie_list)
