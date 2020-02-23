# 爬取动态网页
import requests
from selenium import webdriver
from bs4 import BeautifulSoup
import time


#------------------------------------------------------------
driver = webdriver.Firefox(executable_path=r'/usr/local/bin/geckodriver')
driver.implicitly_wait(20) # 隐性等待，最长等20秒

link = 'http://www.santostang.com/2018/07/04/hello-world/'  #该网页的评论部分是动态加载的
# 1.tar vxzf geckodriver-v0.26.0-linux64.tar.gz
# 2.sudo mv geckodriver /usr/local/bin/
driver.get(link)
#下面driver的查找就是基于渲染之后的内容查找了

#渲染之后的 html 源码,已经和网页右键->查看源码 得到的结果稍有不同了(iframe)
#print(driver.page_source)
#with open("./text.html", "a") as fw:
#    fw.write(driver.page_source)

#------------------------------------------------------------
#driver.maximize_window()

for page in range(0,5):
    driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")

    #使 driver 重新指向那个 frame ???
    driver.switch_to.frame(driver.find_element_by_css_selector("iframe[title=livere]"))

    #load_more = driver.find_element_by_css_selector('button.more-btn')
    load_more = driver.find_element_by_css_selector('button[data-page="{}"]'.format(page+1))
    #xpath = "//button[@data-page='" + str(page+1) + "']" # 定位翻页按钮
    #load_more = driver.find_element_by_xpath(xpath)
    load_more.click()

    time.sleep(5)


    #使用css选择器查找元素,找到 class 为 reply-content 的 div 元素
    comments = driver.find_elements_by_css_selector('div.reply-content')    #注意是 elements,比前面多了个s
    for each_comment in comments:
        #通过元素的 tag 寻找 comment 中的 p 元素
        content = each_comment.find_element_by_tag_name('p')
        #输出p元素中的text文本
        print(content.text)

    driver.switch_to.default_content()





## 定义请求头的浏览器代理,伪装成浏览器
#headers = {
#    'User-Agent' : 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:65.0) Gecko/20100101 Firefox/65.0',
#    'Host' : 'movie.douban.com'
#}
#
#movie_list = []
#for i in range(0,10):
#    # 手动处理 get 请求的 param
#    link = 'https://movie.douban.com/top250?start=' + str(i*25) + '&filter='
#    r = requests.get(url=link, headers=headers, timeout=10)
#    
#    print("{} 页响应码 {}".format(i, r.status_code))
#    #print(r.text)   # r.text 是网页的源码 html文档
#
#    #soup = BeautifulSoup(r.text, "html.parser")
#    #title = soup.find("h1", class_="post-title").a.text.strip()
#    soup = BeautifulSoup(r.text, "lxml")
#    div_list = soup.find_all("div", class_="hd")
#    for each in div_list:
#        movie = each.a.span.text.strip()
#        movie_list.append(movie)
#
#print(movie_list)
