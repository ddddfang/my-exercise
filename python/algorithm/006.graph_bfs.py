#图的广度优先搜索,就是每走一步,挨个检查所有突触,是否是target
#或者每走一步,将所有突触存储到队列中,
#每访问完成一个节点,将其从队列移除的同时,将其所有突触添加入队

from collections import deque
#用字典代表一个有向图
graph = {}

#有向图节点的突触用一个list存起来
graph["A"] = ["B","C","D"]
graph["B"] = ["E","F"]
graph["C"] = ["G","H"]
graph["D"] = []
graph["E"] = []
graph["F"] = []
graph["G"] = ["H","F"]
graph["H"] = ["A","E","F"]


def target(name):
    if name == "H":
        return True
    else:
        return False

#这个只能解决 可不可达 的问题.哪个路径最优?貌似深度优先搜索更容易找到这条path
def bfs_search_from(name):
    search_queue = deque()
    search_queue += graph[name] #入队
    searched = []   #存放已经访问过的节点,防止死循环

    while search_queue:
        item = search_queue.popleft()   #出队
        if item not in searched:
            if target(item):
                print("{}:got it.".format(item))
                return True
            else:
                search_queue += graph[item]
                searched.append(item)
                if not graph[item]: #该item已经没有突触了,那么这条path废了
                    print("{}:game over.".format(item))
    return False

#递归实现的深度优先搜索,先访问再递归下去
def dfs(graph, start):
    visited = set()
    
    #返回:本节点能否到达target节点,若能,返回本节点到target的path
    def recurse(item):
        if not graph[item]: #本节点是最终节点,且不是target节点
            if not target(item):
                return False,[]

        if target(item):    #本节点就是target节点
            return True,[item]
            
        visited.add(item)
        for i in graph[item]:
            if i in visited:
                continue
            res,p = recurse(i)
            if res:
                return True, [item] + p
        return False,[]

    return recurse(start)


def dfs_topsort(graph):
    # 对下面的子函数来说,这是全局变量...
    S,res = set(),[]

    # 函数里定义子函数...
    def recurse(item):
        if item in S: # 递归结束条件是:该元素已经被访问过
            return
        S.add(item)
        for v in graph[item]: # 直接使用外层函数的参数,作为全局变量...
            recurse(v)
        # 这属于一路递归到底再访问的那种类型
        res.append(item)

    for k in graph:
        recurse(k)
    res.reverse()

    return res

bfs_search_from("A")
print("-----")
print(dfs(graph,"A"))



