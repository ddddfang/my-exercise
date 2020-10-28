#python表示tree的方法:
#   1.list的list, ['a', [a的左子树], [a的右子树]],
#   然后a的左右子树也是类似的结构, 叶子节点是类似 ['e',[],[]] 这样的结构
#   (这种看起来是python特有的表示结构)
#   2.定义节点,其中的 left/right 也是这样的节点(面向对象都是这种定义方式)


#只是用于构建树的结构,并不是二叉搜索树
class BTree:
    def __init__(self, rootObj):
        self.key = rootObj
        self.leftChild = None
        self.rightChild = None
    def insertLeft(self, newObj):
        if self.leftChild == None:
            self.leftChild = BTree(newObj)
        else:
            t = BTree(newObj)
            t.leftChild = self.leftChild
            self.leftChild = t
    def insertRight(self, newObj):
        if self.rightChild == None:
            self.rightChild = BTree(newObj)
        else:
            t = BTree(newObj)
            t.rightChild = self.rightChild
            self.rightChild = t
    def setRootVal(self, obj):
        self.key = obj
    def getRootVal(self):
        return self.key
    def getLeftChild(self):
        return self.leftChild
    def getRightChild(self):
        return self.rightChild

class Stack:
    def __init__(self):
        self.items = []
    def __str__(self):
        strItems = list(map(str, self.items)) 
        return "<=".join(strItems)
    def isEmpty(self):
        return self.items == []
    def push(self, item):
        self.items.append(item) #self.items.insert(0, item)则是在第0个元素前插入item
    def pop(self):
        return self.items.pop() #self.items.pop(0)则是弹出第0个元素
    def peek(self):
        return self.items[len(self.items) - 1]
    def size(self):
        return len(self.items)

#解析运算表达式, eg a*(b+c) + (d-e)/f
def buildParseTree(fpexp):
    fplist = fpexp.split()
    s = Stack()
    etree = BTree('')
    s.push(etree)

    ctree = etree

    for i in fplist:
        if i == '(':
            ctree.insertLeft('')    #遇到(肯定要下沉,至于马上出现的是个什么(数or操作符),不知道,所以先用''占位
            s.push(ctree)
            ctree = ctree.getLeftChild()
        elif i not in '+-*/)':
            ctree.setRootVal(eval(i))
            parent = s.pop()
            ctree = parent
        elif i in '+-*/':
            ctree.setRootVal(i)
            ctree.insertRight('')
            s.push(ctree)
            ctree = ctree.getRightChild()
        elif i == ')':
            ctree = s.pop()
        else:
            raise ValueError('unknow operator '+i)
    return etree

if __name__ == "__main__":
    r = BTree('a')
    print(r.getRootVal())
    r.insertLeft('b')
    r.insertRight('c')
    print(r.getLeftChild().getRootVal())
    print(r.getRightChild().getRootVal())
    r.getRightChild().setRootVal('d')
    print(r.getRightChild().getRootVal())
    #
