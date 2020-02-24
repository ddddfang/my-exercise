# 爬取动态网页
import requests
from selenium import webdriver
from bs4 import BeautifulSoup
import time


#------------------------------------------------------------
driver = webdriver.Firefox(executable_path=r'/usr/local/bin/geckodriver')
driver.implicitly_wait(20) # 隐性等待，最长等20秒

link_base = 'https://www.airbnb.cn/s/%E4%B8%8A%E6%B5%B7/homes?refinement_paths%5B%5D=%2Fhomes&current_tab_id=home_tab&selected_tab_id=home_tab&map_toggle=false&screen_size=large&hide_dates_and_guests_filters=false&checkin=2020-02-24&checkout=2020-03-02&host_rule_type_ids%5B%5D=1&place_id=ChIJMzz1sUBwsjURoWTDI5QSlQI&items_offset='
for i in range(0,10):

    print("page{}".format(i+1))
    link = link_base + str(20*i)
    # 1.tar vxzf geckodriver-v0.26.0-linux64.tar.gz
    # 2.sudo mv geckodriver /usr/local/bin/
    driver.get(link)
    #print(driver.page_source)

    #------------------------------------------------------------
    #driver.maximize_window()

    rent_list = driver.find_elements_by_css_selector('div._gig1e7')
    for each_rent in rent_list:
        try:    #查找评论人数
            comment = each_rent.find_element_by_css_selector('span._69pvqtq')  #注意下面的 find 都是在 each_rent 中!
            comment = comment.text
        except:
            comment = 0

        #查找价格
        try:
            price = each_rent.find_element_by_css_selector('span._185kh56')
            price = price.text
        except:
            price = '0'
        #查找名称
        try:
            name = each_rent.find_element_by_css_selector('div._qrfr9x5')
            name = name.text
        except:
            name = ''

        #查找类型和数量
        try:
            details = each_rent.find_element_by_css_selector('div._1etkxf1')
            details = details.text
            #house_type = details.split('.')[0]
            #bed_number = details.split('.')[1]
        except:
            detail = ''

        print("名称:{},价格:{},评论数量:{},详细:{}".format(name, price, comment, details))
    
    time.sleep(10)




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
