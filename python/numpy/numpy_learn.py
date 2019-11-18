


import numpy as np

#利用 range() 产生一个 list = [0,1,2,...29] 来初始化array
#a = np.arange(30,dtype=np.int16).reshape(2, 3, 5)
a = np.arange(30).reshape(2, 3, 5)
print("a=",a)
print("a.dim=",a.ndim)
print("a.shape=",a.shape)
print("a.dtype=",a.dtype.name)
print("a.itemSize=",a.itemsize)
print("a.size=",a.size)#

a = np.linspace(1.0, 5.0, 10, endpoint=False).reshape(2, 5) #在1.0-->5.0 按均匀间隔生成10个点,endpoint 默认为true表示包含5
print(a)
a = np.arange(10,30,0.5)    #step=0.5
print(a)

# 全0数组
a = np.zeros((2,3),dtype=np.int16)
# 全1数组
a = np.ones((2,3))  # dtype default is float64

# 产生随机数(0,1)
a = np.random.random((2,3))
a *= 10
print(a)

# 需要通过 列表/元组 来初始化 np.array
# 这样是错误的-->  array(1,2,3,4)
#a = np.array([(1,2,3),(4,5,6)],dtype=np.complex)
a = np.array([(1,2,3),(4,5,6)])
print(a)
print(a.sum(axis=0)) #对a的每一列求sum，得到一个数组,reduce
print(a.min(axis=1)) #对a的每一行求min，得到一个数组,reduce
print(np.exp(a))	#map操作,还有sin(a),cos(a),sqrt(a),add(a,b)也可以这样


b = np.arange(0,3,0.5).reshape(2,3)
print(b)

# 矩阵 eltwise-add
print(a+b)

# 矩阵 eltwise-mul
print(a*b)

# 矩阵 dot-mul
print(np.dot( a,b.reshape(3,2) ))


a = np.random.random((1,3))
b = np.random.random((2,1))
print(a)
print(b)
c = np.greater(a, b)
print(c)


   
f = np.array([1,2,3])
g = np.array([4,5,6])

print('水平方向:', np.hstack((f, g)))
print('竖直方向:', np.vstack((f, g)))

#水平方向: [1 2 3 4 5 6]
#竖直方向: [[1 2 3]
#          [4 5 6]]
