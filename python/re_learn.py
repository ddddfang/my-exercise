

#一个例子:提取存档的html文件中所有的网址
# http:// 开头,或者 https:// 开头,然后是任意一个字符,(重复任意次),网址一般被""包起来

import re

p = re.compile(r'"https?://.*?"', re.IGNORECASE) # ?" 表示找到离前面一个"最近的"之后就不再继续匹配
#p = re.compile('"http://.*?"|"https://.*?"', re.I) # re.IGNORECASE 和 re.I 都表示忽略大小写

with open("/home/fang/桌面/demo.html") as fr:
    doc = fr.read()
    print(type(doc))
    # findall() 找到所有匹配的字串,作为一个list返回
    # search()  扫描字符串,找到RE匹配的位置(只返回第一个的位置)
    # match() 会返回一个match对象
    for i in p.findall(doc):
        print (type(i), i)

# 提取函数的名字 ? func (x,y) { }
p = re.compile(r'\(.*?\)\s*?\{.*?\}', re.S) #使用单行模式,这种情况下\n也被当作普通字符 https://deerchao.cn/tutorials/regex/regex.htm#greedyandlazy 
#p = re.compile(r'\bR.*?\b', re.S) #使用单行模式,寻找以R开头的所有单词,尽可能少匹配
with open("/home/fang/codes/zxnn_lib/test/test_zxnn_auto_fused.h") as fr:
    doc = fr.read()
    #print(type(doc),doc)
    # findall() 找到所有匹配的字串,作为一个list返回
    # search()  扫描字符串,找到RE匹配的位置(只返回第一个的位置)
    # match() 会返回一个match对象
    for i in p.findall(doc):
        print (type(i), i)

content = "hello , this is a test string"
p = re.compile(r'\w*is', re.I)
for i in p.findall(content):
    print(i)

for i in range(len(content)):
    res = p.match(content,i)  # 默认从pos=0开始匹配,pos=0处匹配不上就认为匹配失败,endpos=len(string)为默认的停止匹配位置
    if res:
        print("found,{},{}".format(res.group(),res.span()))
    else:
        print("not found at {}".format(i))

res = p.search(content)     # search 直接从给定字符串中查找匹配的起始位置,找到就返回不再继续找了
if res:
    print("found,{},{}".format(res.group(),res.span()))
else:
    print("not found")

res = list(p.finditer(content)) # 返回所有匹配的位置 span()
print(res)
for i in res:
    print("type {}, {}, {}".format(type(i), i, i.span()))
