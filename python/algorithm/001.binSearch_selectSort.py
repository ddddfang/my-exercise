import random
############################## 二分查找 #######################################
#只是作为一个二分法演示练习,检查一个元素是否在list中可以用 in 方法,千万不要造这种轮子
#lst必须是有序的
def binary_search(lst, item):
    low = 0
    high = len(lst) - 1
    while low <= high:
        mid = (low + high) // 2
        guess = lst[mid]
        if guess == item:
            return mid
        if guess > item:
            high = mid - 1
        else:
            low = mid + 1
    return None


li = [ i**2 for i in range(10)]
print(li)
print(binary_search(li,4))

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


############################## 递归练习 #######################################
def fact(x):
    if x <=0:
        return 0
    if x == 1:
        return 1
    else:
        return x*fact(x-1)

print(fact(5))
print(fact(-3))
