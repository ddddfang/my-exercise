from __future__ import print_function
import torch
import numpy as np

#x = torch.empty(5, 3)
#print(x)
#
#x = torch.rand(5, 3)
#print(x)
#
#x = torch.zeros(5, 3, dtype=torch.long)
#print(x)
#
#x = torch.tensor([5.5, 3])
#print(x)
#
#x = x.new_ones(5, 3, dtype=torch.double)  # new_* methods
#print(x)
#print(x.size())
#
#y = torch.randn_like(x, dtype=torch.float)
#print(y)
#print(x+y)
#print(torch.add(x,y))
#result = torch.empty_like(x)
#torch.add(x,y,out=result)
#print(result)

#a = torch.ones(5)
#print(a)
#b = a.numpy()
#print(b)
#
#a.add_(1)
#print(a)
#print(b)


#a = np.ones(5)
#b = torch.from_numpy(a)
#np.add(a, 1, out=a)
##a = a+1
#print(a)
#print(b)

print(torch.cuda.is_available())
x = torch.rand(5, 3, dtype=torch.float)
print("x is ", x)
if(torch.cuda.is_available()):
    device = torch.device("cuda")
    y = torch.ones_like(x, device=device)
    x = x.to(device)
    z = x + y
    print("z is",z)
    print(z.to("cpu", torch.double)) # copy + convert


a = torch.arange(0, 14., requires_grad=True)
b = torch.nn.functional.adaptive_avg_pool1d(a[None, None], 4)
print("a is ",a)
print("b is ",b)

## target output size of 5x7
#m = torch.nn.AdaptiveAvgPool2d((5,7))
#x = torch.randn(1, 3, 8, 9)
#output = m(x)
#print("x is ", x)
#print("o is ", output)
#
## target output size of 7x7 (square)
#m = torch.nn.AdaptiveAvgPool2d(7)
#x = torch.randn(1, 3, 10, 9)
#output = m(x)
#print("x is ", x)
#print("o is ", output)
#
## target output size of 10x7
#m = torch.nn.AdaptiveMaxPool2d((None, 7))
#x = torch.randn(1, 3, 10, 9)
#output = m(x)
#print("x is ", x)
#print("o is ", output)

#m = torch.nn.AdaptiveAvgPool1d(3)
#x = torch.arange(0, 2.0*6.0).reshape(1,2,6)
#output = m(x)
#print("x is ", x)
#print("o is ", output)

m = torch.nn.AdaptiveAvgPool2d((3,3))
x = torch.arange(0, 1.0*5.0*5.0).reshape(1,1,5,5)
#x = torch.rand(1,1,5,5)
output = m(x)
print("x is ", x)
print("o is ", output)






import math
import unittest
import numpy as np
import torch
from torch import Tensor
from torch.autograd import gradcheck
from torch.jit.annotations import Tuple
from torch.nn.modules.utils import _pair
from torchvision import ops



x = np.arange(2*3*7*7).reshape(2, 3, 7, 7)
tensorX = Tensor(x)
print(tensorX)

rois = np.zeros(shape=(2,5),dtype=np.int16)
rois[0][0] = 0
rois[0][1] = 3
rois[0][2] = 1
rois[0][3] = 3
rois[0][4] = 1

rois[1][0] = 1
rois[1][1] = 2
rois[1][2] = 2
rois[1][3] = 4
rois[1][4] = 3
tensorRois = Tensor(rois)
print(tensorRois)

#tensorY = ops.RoIAlign((2, 2), spatial_scale=1.0, sampling_ratio=-1)(tensorX, tensorRois)
tensorY = ops.RoIAlign((3, 3), spatial_scale=1.0, sampling_ratio=2)(tensorX, tensorRois)
print(tensorY)



