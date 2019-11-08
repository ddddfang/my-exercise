# -*- coding: utf-8 -*-
# @Time    : 2017/8/2 10:53
# @Author  : play4fun
# @File    : test_camera.py
# @Software: PyCharm
# https://blog.csdn.net/Programmer_ch/article/details/89057129


"""
test_camera.py:
"""

import numpy as np
import cv2
import time
import os
import sys

sys.path.append('./')
import utils   #./utils.py
print("=============================================")
print("=  x: 拍摄图片                              =")
print("=  q: 退出                                  =")
print("=============================================")

#opencv/modules/videoio/src/cap_v4l.cpp (802) open VIDEOIO: V4L: open camera by index 0
#cap = cv2.VideoCapture(0)  # 支持读取摄像头
cap = cv2.VideoCapture("./testvideo/Warcraft3_End.avi") # 调用存储的视频

cap.set(cv2.CAP_PROP_FRAME_WIDTH, 640)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 480)
#cap.set(cv2.CAP_PROP_FOURCC, cv2.VideoWriter.fourcc('M', 'J', 'P', 'G'))

fps = cap.get(cv2.CAP_PROP_FPS) # 帧率
print("fps:", fps)

index = 0
while cap.isOpened():
    ret, frame = cap.read()

    #frameIndex = cap.get(cv2.CAP_PROP_POS_FRAMES)  # 第i帧
    #print("frame index ", frameIndex)
    if ret != True:
        break

    ############################################################
    frame2 = cv2.flip(frame, 1, dst=None)  # 镜像flip
    frame3 = frame[10:10+320, 10:10+240] # crop
    frame4 = utils.salt(frame, 500)
    frame5 = utils.calcAndDrawHist2(frame) #直方图
    cv2.imshow("test", frame4)
    cv2.imshow("test5",frame5)

    ############################################################

    inputKey = cv2.waitKey(5) & 0xFF
    if inputKey == ord('q'):
        break
    elif inputKey == ord('x'):
        cv2.imwrite("{}.jpeg".format(time.time()), cv2.resize(frame, (224, 224), interpolation=cv2.INTER_AREA))
    ############################################################
    time.sleep(0.01)

cap.release()
cv2.destroyAllWindows()

