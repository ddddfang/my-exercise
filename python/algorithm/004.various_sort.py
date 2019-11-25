import random


#冒泡排序：双重for循环
def bubble_sort(nums):
    #num 必须是list且非空
    if not isinstance(nums,list) or not nums:
        return -1
    for i in range(1,len(nums)):    # i 表示需要多少轮冒泡
        #冒泡排序, 第一轮 j->[0 len-1) 最后一轮 j->[0 1)
        for j in range(0,len(nums)-i):
            if nums[j] > nums[j+1]:
                nums[j],nums[j+1] = nums[j+1],nums[j]
        return nums

############################## 选择排序 #######################################
#找到数组的最小值的index
def find_smallest(arr):
    smallest = arr[0]
    smallest_index = 0
    for i in range(1, len(arr)):
        if arr[i] < smallest:
            smallest = arr[i]
            smallest_index = i
    return smallest_index

def select_sort(arr):
    new_array = []
    for i in range(len(arr)):
        smallest_index = find_smallest(arr)
        new_array.append(arr.pop(smallest_index))
    return new_array

#li = [10*random.random() for i in range(10)]
li = [random.randint(0,50) for i in range(10)]
print (li)

new_li = select_sort(li)
print(new_li)

############################## 基数排序 #######################################
#演化0:待排数组num[],bucket[num[i]] = 1, 然后遍历 bucket
#演化1:桶排序
#演化2:
#基数排序,先按个位0-9排序,再按十位0-9排序,....
#结果就是: 数组是局部有序的, 除非 高位原因 打破原有顺序
# https://blog.csdn.net/weizhifei1234/article/details/89066268
def radix_sort(nums):
    max_number = max(nums)
    n = 1
    while max_number >= 10**n:#在这里要设置为等于号，因为我们在后面无法取到n
        n += 1
    #产生0~9之间的10个桶
    for k in range(n):#k在这里是控制位数的
        buckets=[[] for i in range(10)]#桶要在这里产生
        for subnum in nums:
            buckets[int(subnum / (10**k) % 10)].append(subnum)#这里就控制了个位十位等
            nums=[num for bucket in buckets for num in bucket]
    return nums

li = [random.randint(0,50) for i in range(10)]
print("{} ===>> {}".format(li, radix_sort(li)))


################# 一个简单的快速排序实现,只是作为练习用 ########################
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
    if(start >= end):
        return start
    target_arr = arr[start:end]
    #pivot_index = 0     # pivot_index 是在 target_arr 中的 index
    pivot_index = random.randint(0,end-start-1)     # pivot_index 是在 target_arr 中的 index
    pivot = target_arr.pop(pivot_index) #枢纽应该躺在左右分区的交界处,最好移走
    l = [ x for x in target_arr if x <= pivot]
    r = [ x for x in target_arr if x >  pivot]

    arr[start : end] = l + [pivot] + r
    return start + len(l)     # 返回枢纽所在的位置

def quick_sort2(arr,start,end):
    if(end-start < 2):
        return
    else:
        i = partition(arr, start, end)
        quick_sort2(arr, start, i)
        quick_sort2(arr, i+1, end)

#返回第 排序数组第 k 个数(第 k+1 小的数)
def get_Kth(arr, k):
    s = 0
    e = len(arr) - 1
    i = partition(arr, s, e)
    while i != k:
        if i > k:
            e = i - 1
            i = partition(arr, s, e)
        elif i < k:
            s = i + 1
            i = partition(arr, s, e)
    return arr[i]


li = [random.randint(0,50) for i in range(10)]
li_cpy = li[:]
print("{} ===>> {}".format(li,quick_sort(li)))
quick_sort2(li_cpy, 0, len(li))
print(li_cpy)
for i in range(len(li)):
    print("{} th is {}".format(i, get_Kth(li, i)))

############################## 归并排序 #######################################
def merge_sort(nums):
    if not isinstance(nums,list) or not nums:#看这里是否是list变量
        return -1
    n = len(nums)
    if n == 1:
        return nums
    mid = n // 2
    left_list = merge_sort(nums[:mid])
    right_list = merge_sort(nums[mid:])
    return merge(left_list,right_list)

def merge(left_list,right_list):
    left = 0
    right = 0
    merge_result = []
    while left < len(left_list) and right < len(right_list):
        if left_list[left] <= right_list[right]:
            merge_result.append(left_list[left])
            left += 1
        else:
            merge_result.append(right_list[right])
            right += 1
    merge_result += left_list[left:]
    merge_result += right_list[right:]
    return merge_result

li = [random.randint(0,50) for i in range(10)]
print("{} ===>> {}".format(li, merge_sort(li)))


############################## heap(优先级队列) ###################################
#入队:从底部插入,叶子节点可能比父节点大,就需要shit_up
#出队:一般是弹出堆顶元素后(用底层叶子节点覆盖到堆顶,队列len--),需要shift_down
#ps:添加元素到大顶堆没必要一定从队尾入队,加入到root再shift_down也是ok
#引申:
#求得分最高的10个元素:构建小顶堆,大于堆顶就弹出堆顶并插入元素 (也就是替换堆顶啦)
#堆排序n个元素就是构建一个n个元素的大顶堆,每次弹出一个堆顶(放在数组末尾)

#shiftDown:调整 (pos,pos左子节点,pos右子节点) 小子树符合大顶堆,并会向下调整到叶子节点
def shift_down(arr, pos):
    left = 2 * pos + 1  #左子节点index
    right = 2 * pos + 2 #左子节点index
    max_pos = pos   #大顶堆默认max为root

    if(pos > len(arr) // 2 - 1): #大顶堆只有index <= len/2-1的元素才有子节点
        return
    # 找出 (pos,pos左子节点,pos右子节点) 中的 max 和 对应的 index
    if left < len(arr) and arr[left] > arr[max_pos]:
        max_pos = left
    if right < len(arr) and arr[right] > arr[max_pos]:
        max_pos = right

    # 走到这里 max_pos 就是三者中最大元素的 index 了
    if max_pos != pos:
        arr[pos],arr[max_pos] = arr[max_pos],arr[pos]
        shift_down(arr, max_pos) #

#向上调整只要找到本节点和父节点两个元素中的max_pos即可
def shift_up(arr, pos):
    while pos > 0:
        if arr[pos] > arr[(pos - 1) // 2]:
            arr[pos], arr[(pos - 1) // 2] = arr[(pos - 1) // 2], arr[pos]
        else:
            break   #本三角已经满足大顶堆条件,则没必要继续了,因为本身就是大顶堆
        pos = (pos - 1) // 2


def maxheap_make(arr):
    i = len(arr) // 2 - 1   #最后一个有子节点的
    while i >= 0:
        shift_down(arr, i)  #每次相当于添加一个新元素到root,并调整
        i -= 1

def maxheap_sort(arr):
    maxheap_make(arr)   #将 arr 搞成大顶堆
    i = len(arr) - 1
    while i >= 0:
        arr[0], arr[i] = arr[i], arr[0] #swap后,i就在正确的位置了,0但是现在0位置处打破大顶堆约束,需要调整

        arr_cpy = arr[0:i]    #[0:i)
        shift_down(arr_cpy, 0)
        arr[0:i] = arr_cpy

        i -= 1

li = [random.randint(0,50) for i in range(10)]
li_cpy = li[:]
maxheap_sort(li_cpy)
print("{} ===>> {}".format(li, li_cpy))

