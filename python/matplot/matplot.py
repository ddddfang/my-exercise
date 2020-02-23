import matplotlib.pyplot as plt
from matplotlib import font_manager

## 默认当前操作对象是 对象1
#plt.plot([1,2,3,4], [2,1,5,6])
#plt.show()


## 绘制折线图,点间用线相连
#plt.figure(1)   # 设定当前操作对象是 对象1
#plt.title("this is figure1 title")
#plt.xlabel("this is x label")
#plt.ylabel("this is y label")
#
#x = range(10)
#l1 = plt.plot(x, x, 'ro')                   # r标识red,o标识点状线. 默认线型是蓝色直线 'b-'
#l2 = plt.plot(x, [i**2 for i in x], 'bs')   # 蓝色正方形
#l3 = plt.plot(x, [i**3 for i in x], 'g^')   # 绿色三角形
#plt.legend((l1[0], l2[0], l3[0]), ('x^1','x^2','x^3'))    #绘制图例
##plt.show()             # 在对象1上所有plot的点都会被flush并show
#
#plt.figure(2)   # 设定当前操作对象是 对象2
#plt.plot([1,2,3,4], [3,1,4,6])
#plt.savefig("./fig02")
#
#plt.figure(1)   # 第二次操作 对象2,会在原来的基础上继续绘制,(除非原操作对象已经被显示或save)
#plt.plot([2,4], [0,2])
#plt.show()  #这会将之前plot的结果全部显示出来,所以包括对象1和对象2
#plt.savefig("./fig01")  # 在对象1上所有plot的点都会被flush并save


# 绘制散点图,点就是点
x = range(10)
plt.scatter(x, x, color='r', marker='*')  # 使用 scatter 绘制散点图, x 轴坐标组成的list, y轴坐标组成的list
plt.scatter(x, [i**2 for i in x], color='g', marker='+')
plt.scatter(x, [i**3 for i in x], color='b', marker='x')
plt.show()


#设置自己的字体
#my_font = font_manager.FontProperties(fname="C:/Windows/Fonts/simsun.ttc")

x = ["itemA", "itemB", "itemC", "itemD"]
y_1 = [991.94, 375.64, 200.48, 73.27]   #ABCD在第一阶段的值
y_2 = [1908.22, 547.61, 466.23, 193.8]  #ABCD在第二阶段的值
y_3 = [1532.87, 525.63, 332.35, 170.57]

y = y_1 + y_2 + y_3
y_A = []    #A 的第123阶段的值
y_B = []
y_C = []
y_D = []
for i in range(len(y)):
    if i % 4 == 0:
        y_A.append(y[i])
    elif i % 4 == 1:
        y_B.append(y[i])
    elif i % 4 == 2:
        y_C.append(y[i])
    else:
        y_D.append(y[i])

bar_width = 0.2
x_A = [i for i in range(3)]         # [1 2 3]
x_B = [i+bar_width for i in x_1]    # [1.2 2.2 3.2]
x_C = [i+bar_width*2 for i in x_1]  # [1.4 2.4 3.4]
x_D = [i+bar_width*3 for i in x_1]  # [1.6 2.6 3.6]

# 设置图像大小
plt.figure(figsize=(20, 8), dpi=80)

# 绘制直方图
plt.bar(x_A, y_A, width=bar_width, label=x[0], color='#87CEFA') # label设置这个柱状图的标签,要调用plt.legend()函数才能显示出来
plt.bar(x_2, y_2, width=bar_width, label=x[1], color='#ADFF2F')
plt.bar(x_3, y_3, width=bar_width, label=x[2], color='#FFD700')
plt.bar(x_4, y_4, width=bar_width, label=x[3], color='#FFFACD')

#plt.legend(prop=my_font, loc=5)
plt.legend() #添加图例

#调整x和y的刻度
x_tick = [i+0.1 for i in x_2]
xtick_label = ['12月7日', '12月8日', '12月9日']

