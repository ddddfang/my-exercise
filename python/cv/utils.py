
import numpy as np
import cv2

#给图像加椒盐噪点
def salt(img, n):
    img_copy = img.copy()
    for k in range(n):
        i = int(np.random.random() * img_copy.shape[0]);
        j = int(np.random.random() * img_copy.shape[1]);
        if img_copy.ndim == 2: 
            img_copy[i, j] = 255
        elif img.ndim == 3: 
            img_copy[i, j, 0]= 255
            img_copy[i, j, 1]= 255
            img_copy[i, j, 2]= 255
    return img_copy


#hist = cv2.calcHist([image],
#        [0],   #使用的通道
#        None,  #没有使用mask
#        [256], #HistSize,使用多少个柱子
#        [0.0, 255.0]) #像素值范围

#返回一个二维图像的直方图图像,使用color颜色绘制
def calcAndDrawHist(image, color):  
    hist= cv2.calcHist([image], [0], None, [256], [0.0, 255.0]) #计算图像的直方图,hist是一个list或一维数组 
    minVal, maxVal, minLoc, maxLoc = cv2.minMaxLoc(hist)  #统计最大值和对应的location
    histImg = np.zeros([256, 256, 3], np.uint8)  #创建直方图image,256*256大小,三通道彩色图像,rgb范围都是0-255
    hpt = int(0.9* 256);  

    for h in range(256):  
        intensity = int(hist[h] * hpt / maxVal) # (hist/maxVal)*hpt 
        cv2.line(histImg, (h,256), (h,256-intensity), color) #画线,(Point.x,Point.y)
    return histImg

#返回一个三维图像的直方图,将其所有通道同时绘制到一张图像上
def calcAndDrawHist2(image):
    h = np.zeros((256,256,3)) #创建用于绘制直方图的全0图像
    bins = np.arange(256).reshape(256,1) #直方图中各bin的顶点位置
    color = [ (255,0,0), (0,255,0), (0,0,255) ] #BGR三种颜色
    for ch, col in enumerate(color):
        originHist = cv2.calcHist([image], [ch], None, [256], [0,256])
        #cv2.normalize(src, dst, ... )
        cv2.normalize(originHist, originHist, 0, 255*0.9, cv2.NORM_MINMAX) #先归一化,再缩放到(0-0.9*255)区间, (originHist / originHist.maxVal) * (0.9*256)
        hist = np.int32(np.around(originHist))
        # hstack()将两个array 左右(horizal)拼凑在一起. list则认为是 size*1 的array
        # vstack()将两个array 左右(vertical)拼凑在一起.
        # column_stack()将两个array 按列拼凑在一起.
        pts = np.column_stack((bins, hist))  # pts=[ [x0,y0], [x1,y1], [x2,y2], [x3,y3], ... ]
        cv2.polylines(h, [pts], False, col)  # 将 pts 这个 map 作为(x,y) 画在h上
    h = np.flipud(h) #上下(up/down)翻转这个图像
    return h

#上面两个是opencv版本的直方图,这个是numpy版本的绘制直方图
def calcAndDrawHist3(image):
    h = np.zeros((300,256,3))
    bins = np.arange(257)
    bin = bins[0:-1]
    color = [ (255,0,0),(0,255,0),(0,0,255) ]

    for ch,col in enumerate(color):
        item = image[:,:,ch]
        N, bins = np.histogram(item, bins) #统计直方图,返回两个数组
        v = N.max()
        N = np.int32(np.around((N*255)/v))  #归一化,缩放到(0-255),四舍五入
        N = N.reshape(256,1)
        pts = np.column_stack((bin, N))     #按列做拼接
        cv2.polylines(h, [pts], False, col)
    h = np.flipud(h)
    return h

