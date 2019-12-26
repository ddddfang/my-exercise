
import numpy as np

##利用 range() 产生一个 list = [0,1,2,...29] 来初始化array
##a = np.arange(30,dtype=np.int16).reshape(2, 3, 5)
#a = np.arange(30).reshape(2, 3, 5)
#print("a{}={}".format(type(a),a))
#print("a.dim=",a.ndim)
#print("a.shape=",a.shape)
#print("a.dtype=",a.dtype.name)
#print("a.itemSize=",a.itemsize)
#print("a.size=",a.size)#

#a = np.random.randint(low=0, high=10, size=5)   #在[0,10)之间产生5个随机整数
#print(a)

#a = np.eye(5)
#print(a)
#print(a[:,::-1])

##对于二维ndarry,强制转换为 list 将得到 list 组成的 list
#a = np.arange(30).reshape(6, 5)
#print("a({},{})=\n{}".format(type(a),a.shape,a))
#li_a = list(a)  #li_a = a.tolist()
#print("li_a({})=\n{}".format(type(li_a),a))

#a = np.linspace(1.0, 5.0, 10, endpoint=False).reshape(2, 5) #在1.0-->5.0 按均匀间隔生成10个点,endpoint 默认为true表示包含5
#print(a)
#a = np.arange(10,30,0.5)    #step=0.5
#print(a)

## 全0数组
#a = np.zeros((2,3),dtype=np.int16)
## 全1数组
#a = np.ones(shape=6)  # dtype default is float64

# 产生随机数(0,1)
a = np.random.randint(10,size=(5,4))
print(a)
b = a.argmax(axis=0)
print("argmax(axis=0) is \n{}".format(b))
#b = np.argsort(a)
b = a.argsort(0)
print("argsort() is \n{}".format(b))

row=b
print("row is \n{}".format(row))
col=[[i for i in range(b.shape[1])] for j in range(b.shape[0])]
print("col is \n{}".format(col))
print(a[row,col])
a.sort(0)
print(a)

#print("(0, 2)(3, 2)(4, 2)is {}".format(a[[0,3,4],2]))
#row = np.array([2,3,4])
#col = np.array([0,1,2])
#print(".(2,0)(3,1)(4,2) is {}.".format(a[row,col]))

## 需要通过 列表/元组 来初始化 np.array
## 这样是错误的-->  array(1,2,3,4)
##a = np.array([(1,2,3),(4,5,6)],dtype=np.complex)
#a = np.array([(1,2,3),(4,5,6)])
#print(a)
#print(a.sum(axis=0)) #对a的每一列求sum，得到一个数组,reduce
#print(a.min(axis=1)) #对a的每一行求min，得到一个数组,reduce
#print(np.exp(a))	#map操作,还有sin(a),cos(a),sqrt(a),add(a,b)也可以这样


#a = np.array([(1,2,3),(4,5,6)])
#b = np.arange(0,3,0.5).reshape(2,3)
#print(b)
#
## 矩阵 eltwise-add
#print(a+b)
#
## 矩阵 eltwise-mul
#print(a*b)
#
## 矩阵 dot-mul
#print(np.dot( a,b.reshape(3,2) ))
#
#
#a = np.random.random((1,3))
#b = np.random.random((2,1))
#print(a)
#print(b)
#c = np.greater(a, b)
#print(c)


   
#f = np.array([1,2,3])
#g = np.array([4,5,6])
#
#print('水平方向:', np.hstack((f, g)))
#print('竖直方向:', np.vstack((f, g)))
#
##水平方向: [1 2 3 4 5 6]
##竖直方向: [[1 2 3]
##          [4 5 6]]
