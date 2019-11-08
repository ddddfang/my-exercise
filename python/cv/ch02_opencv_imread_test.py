# -*- coding: utf-8 -*-
# @Time    : 2017/8/2 10:06
# @Author  : play4fun
# @File    : test1-opencv-python.py
# @Software: PyCharm

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
#img = cv2.imread('data/messi5.jpg', cv2.IMREAD_UNCHANGED)  # 包括图像的 alpha 通道
img = cv2.imread('data/aloeL.jpg', cv2.IMREAD_UNCHANGED)  # 包括图像的 alpha 通道
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
# https://blog.csdn.net/sunny2038/article/details/9097989

histB = utils.calcAndDrawHist(img, [255,0,0]) #统计 b 通道直方图
histG = utils.calcAndDrawHist(img, [0,255,0]) #统计 g 通道直方图
histR = utils.calcAndDrawHist(img, [0,0,255]) #统计 r 通道直方图
histRGB = utils.calcAndDrawHist2(img)
cv2.imshow("histB", histB)
cv2.imshow("histR", histR)
cv2.imshow("histG", histG)
cv2.imshow("histRGB", histRGB)



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

