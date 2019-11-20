import random

#一个简单的快速排序实现,只是作为练习用
#python提供的sort()直接就搞定了,千万不要用这种轮子...
def quick_sort(arr):
    if(len(arr) < 2):
        return arr
    else:
        ri = random.randint(0, len(arr)-1)
        arr[0],arr[ri] = arr[ri],arr[0] # swap(0,ri)

        pivot = arr[0]
        l = [ i for i in arr[1:] if i <= pivot]
        r = [ i for i in arr[1:] if i >  pivot]
        return quick_sort(l) + [pivot] + quick_sort(r)

#下面是一个错误的例子(原因: python传递值还是引用 理解不深)
#def partition(arr):
#    ri = random.randint(0,len(arr)-1)
#    arr[0],arr[ri] = arr[ri],arr[0] # swap(0,ri)
#
#    pivot = arr[0]
#    l = [ x for x in arr[1:] if x <= pivot]
#    r = [ x for x in arr[1:] if x >  pivot]
#    arr[:] = l + [pivot] + r    # 直接写 arr = l + [pivot] + r, 外面的arr中的内容并没有被改动??(外面的标签暂时指向了你给的对象)
#    return len(l)               # 返回枢纽所在的位置
#
#def quick_sort2(arr):
#    if(len(arr) < 2):
#        return
#    else:
#        i = partition(arr)
#        print("-->i={},{}".format(i,arr))
#        quick_sort2(arr[0:i])   #这样写其实是拷贝了一份下去,所以这样只做了一次分区!!
#        quick_sort2(arr[i+1:])

#对arr的[start,end)区间做 分区 处理
#在该区间选取一个值作为枢纽,移动区间元素使得枢纽左边比其小,右边比其大
#左右分割线的位置就是枢纽位置,将此位置返回
def partition(arr,start,end):
    target_arr = arr[start:end]
    #pivot_index = 0     # pivot_index 是在 target_arr 中的 index
    pivot_index = random.randint(0,end-start-1)     # pivot_index 是在 target_arr 中的 index
    pivot = target_arr.pop(pivot_index) #枢纽应该躺在左右分区的交界处,最好移走
    l = [ x for x in target_arr if x <= pivot]
    r = [ x for x in target_arr if x >  pivot]

    arr[start : end] = l + [pivot] + r
    return start+len(l)     # 返回枢纽所在的位置

def quick_sort2(arr,start,end):
    if(end-start < 2):
        return
    else:
        i = partition(arr,start,end)
        quick_sort2(arr,start,i)
        quick_sort2(arr,i+1,end)






li = [random.randint(0,50) for i in range(10)]
print(li)
print(quick_sort(li))
quick_sort2(li, 0, len(li))
print(li)

