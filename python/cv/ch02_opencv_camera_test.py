# -*- coding: utf-8 -*-
# @Time    : 2017/8/2 10:53
# @Author  : play4fun
# @File    : test_camera.py
# @Software: PyCharm

"""
test_camera.py:
"""

import numpy as np
import cv2
import time
import os

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



index = 0
while cap.isOpened():
    ret, frame = cap.read()

    frame2 = cv2.flip(frame, 1, dst=None)  # 镜像flip
    frame3 = frame[10:10+320, 10:10+240] # crop
    cv2.imshow("test", frame)
    cv2.imshow("test2",frame2)

    inputKey = cv2.waitKey(5) & 0xFF
    if inputKey == ord('q'):
        break
    elif inputKey == ord('x'):
        cv2.imwrite("{}.jpeg".format(time.time()), cv2.resize(frame, (224, 224), interpolation=cv2.INTER_AREA))
    time.sleep(0.01)

cap.release()
cv2.destroyAllWindows()

