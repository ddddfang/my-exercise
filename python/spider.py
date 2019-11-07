
######################################################实现一个数据爬虫
import requests
from bs4 import BeautifulSoup

print('成功导入模块')
r = requests.get(url = 'https://book.douban.com/latest')
print(r.encoding, r.status_code)
print(r.url)

# 查看页面内容
#print(r.text)

# 解析网址
soup = BeautifulSoup(r.text,'lxml')
#print(soup)

# 提取标签
#print(soup.head)  # 头部信息
#print(type(soup.title), soup.title) # 标题
#print(type(soup.a), soup.a)  # 提取的第一个a标签

# 标签、属性、元素
print(soup.a.name)
print(soup.a.attrs)
print(soup.a.text)



# 如何查找标签？ → find() /find_all()
# find() → 查找单个标签
test = soup.find('div',class_="grid-12-12 clearfix").find('a',class_='cover').attrs['href']
print(test)

# find_all() → 查找所有标签
urls = soup.find('div',class_="grid-12-12 clearfix").find_all('a')
url_lst = []
for url in urls[::2]:
    url_lst.append(url['href']) # 保存所有url

print(len(url_lst))
print(url_lst[:5])



#######################################
# 创建函数，采集页面信息
def get_data(ui):
    # 访问页面 + 页面解析
    ri = requests.get(url = ui)  
    soupi = BeautifulSoup(ri.text,'lxml')   
