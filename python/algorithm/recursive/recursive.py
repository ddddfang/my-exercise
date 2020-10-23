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
def drawTrangle(points, color, my_turtle):
    my_turtle.fillcolor(color)
    my_turtle.up()
    my_turtle.goto(points[0])
    my_turtle.down()
    my_turtle.begin_fill()
    my_turtle.goto(points[1])
    my_turtle.goto(points[2])
    my_turtle.goto(points[0])
    my_turtle.end_fill()

def getMid(pa, pb):
    return (pa[0]+pb[0])//2,(pa[1]+pb[1])//2

def draw(degree, color, points, my_turtle):
    drawTrangle(points, color[degree], my_turtle)
    if degree > 0:
        draw(degree - 1, color, [points[0], getMid(points[0],points[1]), getMid(points[0],points[2])], my_turtle)
        draw(degree - 1, color, [points[1], getMid(points[1],points[0]), getMid(points[1],points[2])], my_turtle)
        draw(degree - 1, color, [points[2], getMid(points[2],points[0]), getMid(points[2],points[1])], my_turtle)

import turtle
myturtle = turtle.Turtle()
mywin = myturtle.getscreen()

#drawSpiral(myturtle, 100)
#drawTree(myturtle, 50)
points = [(-500,-250), [0,500], [500,-250]]
colors = ['blue', 'red', 'green', 'white', 'yellow', 'orange']
#drawTrangle(points, colors[0], myturtle)
draw(5, colors, points, myturtle)

mywin.exitonclick()

