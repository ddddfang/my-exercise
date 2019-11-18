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

def write_csv_dict(filename, header, rows):
    with open(filename, 'w', newline='') as f:
        f_csv = csv.DictWriter(f, header)
        f_csv.writeheader()
        f_csv.writerows(rows)
#############################################################################################################

headers = ['class','name','sex','height','year']

rows = [
    [1,'xiaoming','male',168,23],
    [1,'xiaohong','female',162,22],
    [2,'xiaozhang','female',163,21],
    [2,'xiaoli','male',158,21]
]

#rows_dict = [
#    {'class':1,'name':'xiaoming','sex':'male','height':168,'year':23},
#    {'class':1,'name':'xiaohong','sex':'female','height':162,'year':22},
#    {'class':2,'name':'xiaozhang','sex':'female','height':163,'year':21},
#    {'class':2,'name':'xiaoli','sex':'male','height':158,'year':21},
#]


abc = read_csv("test.csv")
write_csv("er.csv", abc[0], abc[1:])

row_header = abc[0]
rows_dict = []
for row_i in abc[1:]:
    #把row_i行的内容组织成一个dict
    sub_dict = {}
    for c,col in enumerate(row_i):
        sub_dict[row_header[c]] = col
    rows_dict.append(sub_dict)

write_csv_dict("sh.csv",abc[0],rows_dict)

#############################################################################################################



if __name__ == "__main__":
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

    file_lines = ["this is line1.\n", "this is line two~\n"]
    write_file("abc.txt", file_lines)



