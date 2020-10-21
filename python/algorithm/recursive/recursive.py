# 以递归方式求一个list的元素之和
def listSum(listItem):
    if len(listItem) == 1:
        return listItem[0]
    return listItem[0] + listSum(listItem[1:])

a = [i for i in range(10)]
print(a)
print(listSum(a))

#将整数转换为任意进制的字符串
def toStr(inNum, base):
    numbers = '0123456789abcdef'
    if inNum < base:
        return numbers[inNum]
    else:
        return toStr(inNum//base, base) + numbers[inNum%base]

print(toStr(325,16))
print(toStr(325, 8))

#展示递归的小动画
def drawSpiral(my_turtle, line_len):
    if line_len > 0:
        #drawSpiral(my_turtle, line_len-5)  #可以仔细想想在前面和在后面调用的区别
        my_turtle.forward(line_len)
        my_turtle.right(90)
        drawSpiral(my_turtle, line_len-5)

#画一颗分形树
def drawTree(my_turtle, line_len):
    if line_len > 0:
        my_turtle.forward(line_len)
        drawTree(my_turtle, line_len-25)
        my_turtle.backward(line_len)    #回到起始点

        my_turtle.right(30)
        my_turtle.forward(line_len)
        drawTree(my_turtle, line_len-25)
        my_turtle.backward(line_len)    #回到起始点

        my_turtle.left(60)
        my_turtle.forward(line_len)
        drawTree(my_turtle, line_len-25)
        my_turtle.backward(line_len)    #回到起始点

        my_turtle.right(30)

#绘制谢尔平斯三角形
def drawTrangle
import turtle
myturtle = turtle.Turtle()
mywin = myturtle.getscreen()

#drawSpiral(myturtle, 100)
drawTree(myturtle, 50)
mywin.exitonclick()

