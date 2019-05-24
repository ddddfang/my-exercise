import time
for i in range(0,3):
    print("hello %d,%s,%s"%(i,time.ctime(),time.strftime( "%Y-%m-%d %H:%M:%S" , time.localtime() )))
    time.sleep(1)

############################################################################
import os
print(os.name)
print(os.getcwd())  #
print(os.listdir()) # 返回指定目录下的所有文件和目录名

#os.chdir('C:\\Users\\Hjx\\Desktop\\' )  # 切换到目标路径
#os.remove('text2.txt')  # 删除一个文件

tar_file = "./abc.txt"
if(os.path.exists(tar_file)):
    f_main = open(tar_file,'r')
    print(f_main.read())
    
    #f_main.seek(0)
    #print(f_main.readline()) # 一次性读取一行 
    #f_main.seek(0)
    #print(f_main.readline(4)) # 读取该行的前n个字符

    f_main.seek(0)
    for i,s in enumerate(f_main.readlines()):
        print("%d:%s"%(i,s),end="")
    f_main.close()
else:
    print("%s not found"%(tar_file))
    f = open(tar_file, 'w', encoding = 'utf8')
    f.write("hello world,\n")
    for i in range(0,10):
        f.write("hello %d,%s\n"%(i,time.strftime( "%Y-%m-%d %H:%M:%S" , time.localtime() )))
    f.close()



#######################################pickle模块实现了基本的数据序列和反序列化 可以将对象以文件的形式存放在磁盘上
import pickle
data = {'a':[1,2,3,4], 'b':('string','abc'), 'c':'hello'}
print(data)
pic_file = open( './data.pkl', 'wb')
pickle.dump(data,pic_file)


#f = open( './data.pkl', 'rb')
#st = pickle.load(f)
#print(st)


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
    infors = soupi.find_all('div',class_="detail-frame")
    lst = []
    for i in infors:
        dic = {}
        dic['书名'] = i.find('h2').text.replace('\n','')
        dic['评分'] = i.find_all('p')[0].text.replace('\n','').replace(' ','')
        dic['其他信息'] = i.find_all('p')[1].text.replace('\n','').replace(' ','')
        dic['简介'] = i.find_all('p')[2].text.replace('\n','').replace(' ','')
        lst.append(dic)
    return lst

url = 'https://book.douban.com/latest'
result = get_data(url)
print(result[:3])





