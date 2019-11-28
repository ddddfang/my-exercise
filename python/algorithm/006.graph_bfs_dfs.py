# https://blog.csdn.net/weizhifei1234/category_8791921.html
#用字典代表一个有向图
#graph = {}
#
##有向图节点的突触用一个list存起来
#graph["A"] = ["B","C","D"]
#graph["B"] = ["E","F"]
#graph["C"] = ["G","H"]
#graph["D"] = []
#graph["E"] = []
#graph["F"] = []
#graph["G"] = ["H","F"]
#graph["H"] = ["A","E","F"]

#有向图无向图只是有没有互相连接的区别
graph = {
    'A':['B','C'],
    'B':['A','C','D'],
    'C':['A','B','D','E'],
    'D':['B','C','E','F'],
    'E':['C','D'],
    'F':['D']
}


def target(name):
    if name == "F":
        return True
    else:
        return False


# BFS 广度优先搜索 层序遍历 洪范
# 核心思想: 本节点出队,子节点s入队,如此循环
def BFS(graph,s):   # graph图  s指的是开始结点
    # 需要一个队列, 用list实现
    queue = []
    queue.append(s)
    # 看是否访问过该结点
    searched = set()
    searched.add(s)
    while (len(queue) > 0):
        vertex = queue.pop(0)   # 弹出第一结点
        print(vertex) 
        nodes = graph[vertex]
        for w in nodes:
            if w not in searched:   # 判断是否访问过
                queue.append(w)
                searched.add(w)

# DFS 深度优先搜索 回溯法(会回看前面的节点)
# 核心思想: 本节点出栈,子节点s入栈(接下来的出栈点),所以效果就是不断纵深
def DFS(graph,s):   # graph图  s指的是开始结点
    # 需要一个栈, 用list实现
    stack = []
    stack.append(s)
    # 看是否访问过该结点
    searched = set()
    searched.add(s)
    while (len(stack) > 0):
        vertex = stack.pop()    #这里pop最后一个元素
        print(vertex)
        nodes = graph[vertex]
        for w in nodes:
            if w not in searched:
                stack.append(w)
                searched.add(w)

#递归实现的深度优先搜索,先访问再递归下去
def DFS_path(graph, start):
    #对于下面的子函数来说,这是全局变量,包括外层函数的参数也是全局变量
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

def BFS_path(graph,s):
    queue = []
    queue.append(s)
    back = {}   #记录每一个子节点回去的路
    end = s     #最后的target节点
    searched = set()
    searched.add(s)
    while (len(queue) > 0):
        vertex = queue.pop(0)   # 弹出第一结点

        if(target(vertex)):
            end = vertex
            break

        nodes = graph[vertex]
        for w in nodes:
            if w not in searched:   # 判断是否访问过
                queue.append(w)
                searched.add(w)
                back[w] = vertex
    # 沿着 target 节点找到 start 节点就是我们要的 path 了
    if end != s:    # 表示我们找到了
        path = [end]
        prev = back[end]
        while prev != s:
            path.append(prev)
            prev = back[prev]
        path.append(s)
        path.reverse()
        return True,path
    return False,[]

BFS(graph,"A")
print("-----")
DFS(graph,"A")
print("-----")
print(DFS_path(graph,"A"))
print("-----")
print(BFS_path(graph,"A"))



# 关于图的应用,一个是连通图中各个节点,并能使得权重最小(多个城市间铺设光缆),这被成为最小生成树算法, Prim & Kruskal.
# 另一个就是两个节点之间的最短路径问题, Dijkstra & Floyd.


