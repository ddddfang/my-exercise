
############################## 二分查找 #######################################
#只是作为一个二分法演示练习,检查一个元素是否在list中可以用 in 方法,千万不要造这种轮子
#lst必须是有序的
#binary_search 的递归版本就是分治算法的一个例子

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

