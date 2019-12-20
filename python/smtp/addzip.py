import zipfile

def addzip(files):
    f = zipfile.ZipFile('results.zip', 'w', zipfile.ZIP_DEFLATED)
    #for i in ["/opt/my_code/test_zip/abc.py", "/opt/my_code/t/新建文本文档.txt"]:
    for i in files:
        filename = i.split('/')[-1]
        f.write(i, filename) #这个file是文件名，意思是直接把文件添加到zip没有文件夹层级， f.write(i)这种写法，则会出现上面路径的层级
    f.close()

tfiles = ["/home/fang/.bashrc", "/home/fang/下载/算法新解.pdf"]

addzip(tfiles)

