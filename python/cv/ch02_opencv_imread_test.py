# -*- coding: utf-8 -*-
# @Time    : 2017/8/2 10:06
# @Author  : play4fun
# @File    : test1-opencv-python.py
# @Software: PyCharm

# https://blog.csdn.net/sunny2038/article/details/9097989
"""
test-opencv-python.py:
"""

import numpy as np
import cv2
#from matplotlib import pyplot as plt
import sys

sys.path.append('./')
import utils   #./utils.py


print(cv2.__version__, cv2.__doc__)



############################################### 加载一幅原始图像并显示 #########################################################
img = cv2.imread('data/messi5.jpg', cv2.IMREAD_UNCHANGED)  # 包括图像的 alpha 通道
#img = cv2.imread('data/aloeL.jpg', cv2.IMREAD_UNCHANGED)  # 包括图像的 alpha 通道
#img = cv2.imread('data/messi5.jpg')

h, w, c = img.shape
print('行/高:', h, '列/宽:', w, '通道:', c)
cv2.imshow('src', img) 
cv2.moveWindow('src', 0, 0)

############################################### 通道分离与合并 #########################################################

#img = utils.salt(img, 500)  #访问像素,加噪点

#b, g, r = cv2.split(img)
#cv2.imshow("Red", r)  #这里显示的时候本来就应该是灰度图像,代表了red元素的多少
#cv2.imshow("Green", g)
#cv2.imshow("Blue", b)

b = np.zeros((img.shape[0], img.shape[1]), dtype = img.dtype)
g = np.zeros((img.shape[0], img.shape[1]), dtype = img.dtype)
r = np.zeros((img.shape[0], img.shape[1]), dtype = img.dtype)
b[:,:] = img[:, :, 0]
g[:,:] = img[:, :, 1]
r[:,:] = img[:, :, 2]
#cv2.imshow("Red", r)  #这里显示的时候本来就应该是灰度图像,代表了red元素的多少
#cv2.imshow("Green", g)
#cv2.imshow("Blue", b)
merged = cv2.merge([b, g, r]) # merged = np.dstack([b, g, r]) #this merged result cannot be used to other opencv func because layout difference.
#cv2.imshow("merge", merged)


############################################### 计算并显示直方图 ########################################################

histB = utils.calcAndDrawHist(img, [255,0,0]) #统计 b 通道直方图
histG = utils.calcAndDrawHist(img, [0,255,0]) #统计 g 通道直方图
histR = utils.calcAndDrawHist(img, [0,0,255]) #统计 r 通道直方图
histRGB = utils.calcAndDrawHist3(img)
#cv2.imshow("histB", histB)
#cv2.imshow("histR", histR)
#cv2.imshow("histG", histG)
#cv2.imshow("histRGB", histRGB)

############################################### 形态学 #########################################################
kernel = cv2.getStructuringElement(cv2.MORPH_RECT,(3, 3))

#腐蚀图像
eroded = cv2.erode(img,kernel)
#cv2.imshow("Eroded Image",eroded)

#膨胀图像
dilated = cv2.dilate(img,kernel)
#cv2.imshow("Dilated Image",dilated)

NpKernel = np.uint8(np.ones((3,3)))#NumPy定义的结构元素
Nperoded = cv2.erode(img,NpKernel)
#cv2.imshow("Eroded by NumPy kernel",Nperoded);

closed = cv2.morphologyEx(img, cv2.MORPH_CLOSE, kernel) #闭运算=先膨胀后腐蚀
opened = cv2.morphologyEx(img, cv2.MORPH_OPEN, kernel)  #开运算=先腐蚀后膨胀
#cv2.imshow("open",opened)
#cv2.imshow("close",closed)

#在膨胀时，图像中的物体会向周围“扩张”;腐蚀时,图像中的物体会“收缩”. 比较这两幅图像,
#由于其变化的区域只发生在边缘,所以这时将两幅图像相减,得到的就是图像中物体的边缘
edge = cv2.absdiff(dilated, eroded)

retval, edge = cv2.threshold(edge, 40, 255, cv2.THRESH_BINARY)#上面得到的结果是灰度图，将其二值化以便更清楚的观察结果
edge = cv2.bitwise_not(edge); #反色，即对二值图每个像素取反
cv2.imshow("edge", edge)

#与边缘检测不同，拐角的检测的过程稍稍有些复杂。但原理相同，所不同的是先用十字形的结构元素膨胀像素，
#这种情况下只会在边缘处“扩张”，角点不发生变化。接着用菱形的结构元素腐蚀原图像，导致只有在拐角处才会“收缩”，而直线边缘都未发生变化。
#第二步是用X形膨胀原图像，角点膨胀的比边要多。这样第二次用方块腐蚀时，角点恢复原状，而边要腐蚀的更多。所以当两幅图像相减时，只保留了拐角处

###############################################  #########################################################
img = cv2.resize(img, (640, 480))
h, w, c = img.shape
print('行/高:', h, '列/宽:', w, '通道:', c)

gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#cv2.imshow('gray', gray)  
#cv2.moveWindow('gray', 0, 0)

ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
#cv2.imshow('thresh', thresh)
#cv2.moveWindow('thresh', 0, 0)





#可以把结果写入jpg文件
#cv2.imwrite('messi5-gray.jpg', gray)
#cv2.imwrite('messi5-thresh.jpg', thresh)


cv2.waitKey(0)
cv2.destroyAllWindows()

