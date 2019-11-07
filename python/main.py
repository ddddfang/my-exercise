if __name__ == "__main__":
    print("his program is being run by itself")
else:
    print("I am being imported from another module")

l = ["hello","this","is","a","test"]
print(" ".join(l))

import sys

print("command line arguments are:")
for i,arg in enumerate(sys.argv):
    print("args[{0}] is {1}".format(i, arg))
print("\n\n the python path is", sys.path,"\n")


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







