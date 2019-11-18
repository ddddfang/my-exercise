import os


#遍历目录(包括子目录),返回所有符合要求的文件名字
def get_filename(path, filefilter):
    result = []
    for curdir, dirname, filename in os.walk(path):
        #curdir指出当前所在的目录,dirname指出当前目录下的目录文件名字,filename指出当前目录下的普通文件名字
        #walk发现目录时会遍历那个子目录
        #print("hi,{}{}".format(curdir,filename))
        for f in filename:
            for ft in filefilter:
                #检查 f 是否包含 ft 子串
                #if ft in f:
                if os.path.splitext(f)[1] == ft:
                    fullname = curdir + "/" + f
                    result.append(fullname)
    return result



if __name__ == "__main__":
    filetype = [".h","cpp"]
    rootdir = "/home/fang/codes/tmp/my-exercise/"

    print(os.listdir("/home/fang/codes/tmp/")) # 这个和 ls 列出的结果是一样的,返回一个str组成的list
    filenames = get_filename(rootdir, filetype)
    for i in filenames:
        print(i)
 
