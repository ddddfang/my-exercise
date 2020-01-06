import matplotlib.pyplot as plt

## 默认当前操作对象是 对象1
#plt.plot([1,2,3,4], [2,1,5,6])
#plt.show()



plt.figure(1)   # 设定当前操作对象是 对象1
plt.title("this is figure1 title")
plt.xlabel("this is x label")
plt.ylabel("this is y label")
x = range(10)
l1 = plt.plot(x, x, 'ro')
l2 = plt.plot(x, [i**2 for i in x], 'bs')
l3 = plt.plot(x, [i**3 for i in x], 'g^')
plt.legend((l1[0], l2[0], l3[0]), ('1','2','3'))
#plt.show()             # 在对象1上所有plot的点都会被flush并show

plt.figure(2)   # 设定当前操作对象是 对象2
plt.plot([1,2,3,4], [3,1,4,6])
plt.savefig("./fig02")

plt.figure(1)   # 第二次操作 对象2
plt.plot([2,4], [0,2])
plt.show()  #这会将之前plot的结果全部显示出来
plt.savefig("./fig01")  # 在对象1上所有plot的点都会被flush并save



