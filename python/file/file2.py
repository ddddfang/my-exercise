import os
import csv
import glob
import pandas as pd

def demo():    # read and write xlsx
    files = glob.glob("/home/fang/桌面/*.xls")
    print(files)

    #read
    with pd.ExcelFile(files[0]) as ef: # 带有解析操作
        for sh in ef.sheet_names:
            df = pd.read_excel(ef, sh, skiprows=2)
            #data = df           # 所有数据, pandas.core.frame.DataFrame 格式
            #data = df.values    # 返回一个 numpy.ndarray
            #lines = df.index.values #返回一个一维ndarray

            #data = df.iloc[10:12].values    #读取第[10,12)行内容
            #data = df.iloc[10:12, 0:2].values    #读取第[10,12)行,[0,2)列内容
            print(df.index.values)
            data = df.iloc[0:10:2,:].values
            print(df.values.shape, data.shape, data)
            #print("line10 is {}{}".format(type(data), data))

    # write 一次写入多行, list 的 list 表示一个表格
    df = pd.DataFrame([[1,2,3,4], [5,6,7,8], [9,10,11,12]], index=[0,1,2], columns=list("ABCD"))
    df.to_excel("./abc.xlsx")

if __name__ == "__main__":

    demo()


