

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

