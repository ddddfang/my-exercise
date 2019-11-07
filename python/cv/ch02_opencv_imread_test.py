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

print(cv2.__version__, cv2.__doc__)

img = cv2.imread('data/messi5.jpg', cv2.IMREAD_UNCHANGED)  # 包括图像的 alpha 通道
rows, cols, ch = img.shape
print('行/高:', rows, '列/宽:', cols, '通道:', ch)
cv2.imshow('src', img) 
cv2.moveWindow('src', 0, 0)

img = cv2.resize(img, (640, 480))
rows, cols, ch = img.shape
print('行/高:', rows, '列/宽:', cols, '通道:', ch)

gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
cv2.imshow('gray', gray)  
cv2.moveWindow('gray', 0, 0)

ret, thresh = cv2.threshold(gray, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)
cv2.imshow('thresh', thresh)
cv2.moveWindow('thresh', 0, 0)





#可以把结果写入jpg文件
cv2.imwrite('messi5-gray.jpg', gray)
cv2.imwrite('messi5-thresh.jpg', thresh)


cv2.waitKey(0)
cv2.destroyAllWindows()

