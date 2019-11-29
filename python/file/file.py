import os
import csv

#遍历目录(包括子目录),返回所有符合要求的文件名字
def get_filename(path, filefilter):
    result = []
    for curdir, dirname, filename in os.walk(path):
        #curdir指出当前所在的目录,dirname指出当前目录下的目录文件名字,filename指出当前目录下的普通文件名字
        #walk发现目录时会遍历那个子目录
        #print("hi,{}{}".format(curdir,filename))
        for f in filename:
            if os.path.splitext(f)[1] in filefilter:
                fullname = os.path.join(curdir, f)
                result.append(fullname)
            #for ft in filefilter:
            #    #检查 f 是否包含 ft 子串
            #    #if ft in f:
            #    if os.path.splitext(f)[1] == ft:
            #        fullname = curdir + "/" + f
            #        result.append(fullname)
    return result


# 直接使用python的glob模块获取某一路径下的所有文件
import glob
#files = glob.glob("/home/fang/codes/tmp/my-exercise/python/*.py")
#files = glob.glob("/home/fang/codes/tmp/my-exercise/python/*/*")


#############################################################################################################
#读取文件,只是一个demo! 作为代码片段
def read_file(filename):
    f = open(filename, "r")
    
    #content = f.read()         #一次性读取所有内容到字符串,这里可以指定读取多少字节
    #content = f.readline(20)   #一次性读取1行内容到字符串,最长读取20bytes
    content = f.readlines()     #readlines 返回一个str(包含\n)组成的list
    print("{} has {} lines.".format(filename, len(content)))
    
    f.close()
    return content[:2]  #这里只要第0行和第1行


def write_file(filename, lines):
    #f = open(filename, "w") #写入方式打开,没有则创建
    f = open(filename, "r+") #貌似可读可写模式的时候 seek才会真正生效?
    #f = open(filename, "a") #append方式写入
    print(f.tell())
    #f.seek(0,0) # seek 到文件开始位置
    #f.seek(0,1) # seek 到文件当前位置
    f.seek(0,2) # seek 到文件末尾位置
    print(f.tell())
    f.writelines(lines)
    print(f.tell())

    f.close()
#############################################################################################################

def read_csv(filename):
    with open(filename) as f:
        f_csv = csv.reader(f)
        result = []
        for row in f_csv:
            #row是这一行的所有列组成的list
            #print(row)
            result.append(row)
        return result

def write_csv(filename, header, rows):
    with open(filename, 'w') as f:
        f_csv = csv.writer(f)
        f_csv.writerow(header)
        f_csv.writerows(rows)

def read_csv_dict(filename):
    with open(filename, 'r') as f:
        f_csv = csv.DictReader(f)
        result = [] # list 每个成员都是一个 OrderedDict
        for row in f_csv:
            result.append(row)
        return result

def write_csv_dict(filename, header, rows):
    with open(filename, 'w', newline='') as f:
        f_csv = csv.DictWriter(f, header)
        f_csv.writeheader()
        f_csv.writerows(rows)


headers = ['class','name','sex','height','year']

rows = [
    [1,'xiaoming','male',168,23],
    [1,'xiaohong','female',162,22],
    [2,'xiaozhang','female',163,21],
    [2,'xiaoli','male',158,21]
]

#这个字典组成的list应该可以通过插入 dict(zip(headers,rows[i]))完成
#rows_dict = [
#    {'class':1,'name':'xiaoming','sex':'male','height':168,'year':23},
#    {'class':1,'name':'xiaohong','sex':'female','height':162,'year':22},
#    {'class':2,'name':'xiaozhang','sex':'female','height':163,'year':21},
#    {'class':2,'name':'xiaoli','sex':'male','height':158,'year':21},
#]


#############################################################################################################
import pandas as pd

#############################################################################################################

def demo1(): # get_filename and read_file
    filetype = [".py",".txt"]
    rootdir = "/home/fang/codes/tmp/my-exercise/"
    
    print(os.listdir("/home/fang/codes/tmp/")) # 这个和 ls 列出的结果是一样的,返回一个str组成的list
    filenames = get_filename(rootdir, filetype)
    for fi in filenames:
        print("process file {} ...".format(fi))
        
        lines = read_file(fi)
        for i,l in enumerate(lines):
            print("show line {}:{}".format(i, l), end='')
        print("done.")

def demo2():    # write_csv and write_csv_dict and read_csv
    write_csv("test.csv", headers, rows)
    abc = read_csv("test.csv")
    print(abc)
    abc = read_csv_dict("test.csv")
    print(abc)
    
    row_header = abc[0]
    rows_dict = []
    for row_i in abc[1:]:
        #把row_i行的内容组织成一个dict
        #sub_dict = {}
        #for c,col in enumerate(row_i):
        #    sub_dict[row_header[c]] = col
        #rows_dict.append(sub_dict)
        rows_dict.append(dict(zip(row_header,row_i)))
    write_csv_dict("test2.csv",abc[0],rows_dict)

# https://www.cnblogs.com/liulinghua90/p/9935642.html
def demo3():    # read and write xlsx
    files = glob.glob("/home/fang/桌面/*.xlsx")
    print(files)

    #read
    for i in files:
        ##df = pd.read_excel(i,sheet_name=0)         #读取第0个sheet
        #df = pd.read_excel(i,sheet_name="Sheet1")   #按名字读取sheet
        ##df = pd.read_excel(i,sheet_name="Sheet1",index_col=0,skiprows=3)#第0列是index列,可以不显示,跳过3行
        ##data = df          # 所有数据, pandas.core.frame.DataFrame 格式
        #data = df.head()    #只取前5行
        ##data = df.values   # 返回一个 numpy.ndarray
        #print("excel data({0}) :{1}.".format(type(data),data))

        with pd.ExcelFile(i) as ef: # 带有解析操作
            for sh in ef.sheet_names:
                df = pd.read_excel(ef, sh)
                #data = df           # 所有数据, pandas.core.frame.DataFrame 格式
                data = df.values    # 返回一个 numpy.ndarray
                print("excel data({0}){1} :{2}.".format(type(data), data.shape, data))

                lines = df.index.values
                print(lines)

                #data = df.ix[10].values    #读取第10行的内容
                #data = df.iloc[10:12].values    #读取第[10,12)行内容
                data = df.iloc[10:12, 0:2].values    #读取第[10,12)行,[0,2)列内容
                print("line10 is {}{}".format(type(data), data))

    # write 一次写入多行, list 的 list 表示一个表格
    df = pd.DataFrame([[1,2,3,4], [5,6,7,8], [9,10,11,12]], index=[0,1,2], columns=list("ABCD"))
    df.to_excel("./abc.xlsx")

if __name__ == "__main__":

    #demo1()
    #demo2()
    demo3()


