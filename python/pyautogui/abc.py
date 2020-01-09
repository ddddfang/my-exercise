# https://www.2cto.com/kf/201709/684137.html
# https://www.jianshu.com/p/b42f262e02a2
# https://www.cnblogs.com/daysn/p/11424980.html

import pyautogui as pgui

# 开启自动防故障功能,将鼠标移到屏幕的左上角可停用代码
pgui.FAILSAFE = True
pgui.PAUSE = 1.5 # 为所有的 pyautogui 函数增加延迟. 默认延迟时间是0.1秒. 在函数循环执行的时候，这样做可以让 pyautogui 运行的慢一点,非常有用

# 获取屏幕的宽高
screen_w, screen_h = pgui.size()
print("{},{}".format(screen_w, screen_h))

# 获取屏幕截图(返回的是一个 PIL 的 Image)
img = pgui.screenshot()	#等效于 img = pgui.screenshot(region=(0, 0, screen_w ,screen_h))
img.save("screen.jpg")	#或直接 pgui.screenshot("screen.png")

# 获取截图中某个点的像素
(r,g,b) = img.getpixel((10,20))
print("r{},g{},b{}".format(r,g,b))

# 判断屏幕坐标的像素是不是等于某个值
ifEqual = pgui.pixelMatchesColor(10, 20, (1, 115, 199))
print(ifEqual)

img_region = pgui.screenshot(region=(0, 0, 300 ,80))
img_region.save("screen2.png")
# 把图片像素和屏幕像素逐个比对(发现这里使用jpg格式不好使??),匹配失败返回 None
# 将返回元组 (startw, starth, w, h) 格式
pos = pgui.locateOnScreen('screen2.png',grayscale=False)
poss = list(pgui.locateAllOnScreen('screen2.png'))	# 若找到多个区域,返回元组组成的list
print("pos:{}. all matched pos:{}.".format(pos, poss))
#x,y = pgui.locateCenterOnScreen('screen2.png') 相当于 pgui.locateOnScreen + pgui.center

# 获得一块区域的中心点
cw, ch = pgui.center((0, 0, screen_w, screen_h))
print("center {},{}".format(cw,ch))
# 查看一个点是否在屏幕上
print(pgui.onScreen(cw, ch)) #True




# 获取当前鼠标的坐标
curMouseW, curMouseH = pgui.position()
print("cur mouse pos is {},{}".format(curMouseW, curMouseH))

# 鼠标点击
#pgui.click()	# 等效于 pgui.click(x=curMouseW, y=curMouseH, button='left')
#pgui.mouseDown()	# 按下鼠标按键
#pgui.mouseUp()		# 释放鼠标按键
pgui.click(x=curMouseW, y=curMouseH, button='right')	#"left", "middle", "right"
pgui.click(x=curMouseW, y=curMouseH, button='left', clicks=2, interval=0.25)	# 双击右键两次,两次单击之间间隔0.25s
pgui.scroll(200)	#正值往上滚

# 鼠标移动
pgui.moveTo(cw,ch,duration=1)  #在duration时间内将鼠标匀速移动到指定位置(绝对位置)
pgui.moveRel(100,200,duration=0.5)	#在duration时间内将鼠标匀速移动到指定位置(相对当前位置)
pgui.dragTo(cw,ch,duration=1)  #拖动到指定位置(绝对位置)
pgui.dragRel(100,200,duration=0.5)	#拖动到指定位置(相对当前位置)
#pgui.moveTo(cw,ch,				2, pgui.easeInQuad)#开始很慢，不断加速
#pgui.moveTo(screen_w,screen_h,	2, pgui.easeOutQuad)#开始很快，不断减速
#pgui.moveTo(cw,ch,				2, pgui.easeInOutQuad)#开始和结束都快，中间比较慢
#pgui.moveTo(screen_w,screen_h,	2, pgui.easeInBounce)#一步一徘徊


# 键盘输入字符串
pgui.click()	#先点击以下获取焦点
pgui.typewrite('Hello world\nthis is a test of typewrite',0.25)	#0.25是打印一个字母后的延时,默认是0
print(pgui.KEYBOARD_KEYS)	# 查看所有的key

pgui.keyDown('a')  # 按下按键
pgui.keyUp('a')  # 释放按键
#pgui.hotkey('Ctrl','c')# 热键组合,顺序按下,再按相反的顺序释放
pgui.hotkey('Ctrl','g')
pgui.typewrite('15',0.25)
pgui.press('\n')  # 点击按键，相当于按下和释放的集合


ret = pgui.alert('这个消息弹窗是文字+OK按钮')	#让程序暂停
print(ret)
ret = pgui.confirm('这个消息弹窗是文字+OK+Cancel按钮')	# 点击OK就返回"OK",点击Cancel就返回"Cancel"
print(ret)
ret = pgui.prompt('这个消息弹窗是让用户输入字符串，单击OK')	#点击OK就返回字符串,点击Cance就返回None
print(ret)




