# -*- coding: utf-8 -*-
# @Time    : 2017/7/28 23:13
# @Author  : play4fun
# @File    : OpenCV图像坐标系_test.py
# @Software: PyCharm

"""
OpenCV图像坐标系_test.py:
"""

# TODO


import numpy as np
import cv2

print(cv2.getBuildInformation())


img = cv2.imread('data/Lenna.png', cv2.IMREAD_UNCHANGED)
print('img.shape:', img.shape)

logo = cv2.imread('data/opencv_logo.png', cv2.IMREAD_UNCHANGED)
logo = cv2.resize(logo, (60, 60))
print('logo.shape:', logo.shape)

butterfly= cv2.imread('data/butterfly.jpg', cv2.IMREAD_UNCHANGED)
butterfly = cv2.resize(butterfly, (40, 40))
print('butterfly.shape:', butterfly.shape)


cv2.imshow('src', img)
cv2.moveWindow('src', 0, 0)

# read color values at position y, x
y = 100
x = 50
(b, g, r) = img[y, x]
# print color values to screen
print('bgr:',b,g,r)

#先行后列
img[100:100 + logo.shape[0], 300:300 + logo.shape[1]] = logo[:, :, 0:3]
img[300:300 + butterfly.shape[0], 100:100 + butterfly.shape[1], 0:3] = butterfly[:, :, 0:3]


font = cv2.FONT_HERSHEY_SIMPLEX
#cv2.putText(img, text='col=w=X0,row=h=Y0', org=(0, 0), fontFace=font, fontScale=0.5, color=(0, 255, 0), thickness=2,bottomLeftOrigin=True)  # text,
cv2.putText(img, text='col=w=X0,row=h=Y0', org=(0, 50), fontFace=font, fontScale=0.5, color=(0, 255, 0), thickness=2)  # text,
cv2.putText(img, text='col=w=X10,row=h=Y30', org=(10, 30), fontFace=font, fontScale=0.5, color=(0, 255, 0), thickness=2)  # text,
cv2.putText(img, text='col=w=X100,row=h=Y300', org=(100, 300), fontFace=font, fontScale=0.5, color=(0, 255, 0), thickness=2)  # text,
cv2.putText(img, text='col=w=X300,row=h=Y100', org=(300, 100), fontFace=font, fontScale=0.5, color=(0, 255, 0), thickness=2)  # text,

cv2.imwrite('img_logo.jpg',img)
cv2.imshow('img+logo', img)
cv2.moveWindow('img+logo', x=img.shape[0], y=0)
cv2.waitKey(0) #不添最后一句，在IDLE中执行窗口直接无响应。在命令行中执行的话，则是一闪而过


cv2.destroyAllWindows()


