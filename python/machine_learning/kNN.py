import numpy as np
import operator
import matplotlib.pyplot as plt


def createDataSet():
    group = np.array([  [1.0, 2.0],
                        [1.2, 0.1],
                        [0.1, 1.4],
                        [0.3, 3.5],
                        [1.1, 1.0],
                        [0.5, 1.5]])
    labels = np.array([ 'A',
                        'A',
                        'B',
                        'B',
                        'A',
                        'B'])
    return group, labels

def kNN_classfy(k, dis, X_train, x_train, Y_test):
    assert dis == 'E' or dis == 'M', 'error dis algorithm !'    # E 表示欧式距离,M表示曼哈顿距离
    num_test = Y_test.shape[0]  # 几个点

    label_list = []
    if (dis == 'E'):
        for i in range(num_test):
            # 计算 test 点到训练点之间的距离,并将他们排序
            distances = np.sqrt(np.sum((X_train - np.tile(Y_test[i], (X_train.shape[0], 1)))**2, axis=1))   # tile 将 Y_test [a, b] 变成 X_train 一样的 shape
            print(distances)
            nearest = np.argsort(distances)
            print(nearest)
            top_k = nearest[:k] # 取前k个
            print(top_k)

            classCnt = {}
            for i in top_k:
                classCnt[x_train[i]] = classCnt.get(x_train[i], 0) + 1
            print(classCnt)
            sortedClassCnt = sorted(classCnt.items(), key=operator.itemgetter(1), reverse=True) # 返回的是 items 组成的 list
            #print (sortedClassCnt)
            label_list.append(sortedClassCnt[0][0])
        return np.array(label_list)


import torch
from torch.utils.data import DataLoader
import torchvision.datasets as dsets
import torchvision.transforms as transforms

def get_mnist():
    batch_size = 100
    train_dataset = dsets.MNIST(root='/home/fang/codes/tmp/my-exercise/python/machine_learning/pymnist',
                             train=True,        # 选择训练集
                             transform=None,    # 不使用任何数据预处理
                             download=True      # 从网络下载图片
                             )

    test_dataset = dsets.MNIST(root='/home/fang/codes/tmp/my-exercise/python/machine_learning/pymnist',
                             train=False,       # 选择测试集
                             transform=None,    # 不使用任何数据预处理
                             download=True      # 从网络下载图片
                             )

    # DataLoader: 组织成 batch + 数据打乱 shuffle + 并行加速
    train_loader = DataLoader(dataset=train_dataset, batch_size=batch_size, shuffle=True)
    test_loader = DataLoader(dataset=test_dataset, batch_size=batch_size, shuffle=True)

    print(train_dataset.data.size())
    print(train_dataset.targets.size()) #labels

    print(test_dataset.data.size())
    print(test_dataset.targets.size())

    digit = train_loader.dataset.data[0]    #取第一个图片的数据
    plt.imshow(digit, cmap=plt.cm.binary)
    plt.show()
    print(train_loader.dataset.targets[0])  #取第一个图片对应的标签


if __name__ == '__main__':
    #group, labels = createDataSet()
    #test = np.array([  [0.3, 2.5],
    #                   [1.0, 1.0],
    #                   [0.2, 1.5]
    #                   ])
    ##plt.scatter(group[:,0], group[:,1], color='r', marker='*')    # 使用 scatter 绘制散点图, x 轴坐标组成的list, y轴坐标组成的list
    #plt.scatter(group[labels=='A', 0], group[labels=='A', 1], color='r', marker='*')   # bool组成的list会对 array 起过滤作用 
    #plt.scatter(group[labels=='B', 0], group[labels=='B', 1], color='g', marker='+')
    #plt.scatter(test[:,0], test[:,1], color='b', marker='x')
    #plt.show()

    #print(group,labels)
    #results = kNN_classfy(3, 'E', group, labels, test)
    #print(results)

    get_mnist()

