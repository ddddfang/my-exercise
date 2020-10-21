
class Queue:
    def __init__(self):
        self.items = []
    def isEmpty(self):
        return self.items == []
    def enqueue(self, item):
        self.items.insert(0, item)  #则是在第0个元素前插入item
    def dequeue(self):
        return self.items.pop()     #self.items.pop(0)则是弹出第0个元素
    def size(self):
        return len(self.items)


#约瑟夫环:30人围坐一圈,每隔7人杀掉一个,问最后留下的那个人是谁
#创建一个size=30的queue,每次出队并插入到队尾,到第七个人的时候出队直接丢弃

#双端队列:队列和栈的结合,元素可以从两端进,也可以从两端出,看使用者如何用
#利用双端队列可以解决的问题:回文检测,madam,从左到右依次进队,从两端同时弹出
#相同就继续,直到全部弹出或仅剩一个元素
