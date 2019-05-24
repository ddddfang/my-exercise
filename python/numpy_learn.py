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

a = np.arange(10,30,0.5)    #step=0.5
print(a)

# 需要同过 列表/元组 来初始化 np.array
# 这样是错误的-->  array(1,2,3,4)
a = np.array([(1.5,2,3),(4,5,6)],dtype=np.complex)
print(a)

a = np.zeros((3,4),dtype=np.int16)
b = np.ones((3,4))  # dtype default is float64
print(a)


