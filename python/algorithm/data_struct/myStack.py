class Stack:
    def __init__(self):
        self.items = []
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


# 1.利用Stack检测左右括号是否匹配
def matchers(open, close):
    opens = '([{'
    closes = ')]}'
    return opens.index(open) == closes.index(close)

def parChecker(symbolString):
    s = Stack()
    balanced = True
    index = 0
    while index < len(symbolString) and balanced:
        symbol = symbolString[index]
        #if symbol == '(' or if symbol == '[' or if symbol == '{':
        if symbol in '([{':
            s.push(symbol)
        #elif symbol == ')' or if symbol == ']' or if symbol == '}':
        elif symbol in ')]}':
            if s.isEmpty():
                balanced = False
            else:
                top = s.pop()
                if not matchers(top, symbol):
                    balanced = False
        index = index + 1

    if balanced and s.isEmpty():
        return True
    else:
        return False

if __name__ == "__main__":
    #
    s = Stack()
    print(s.isEmpty())

    s.push(4)
    s.push('dog')
    s.push(12.3)

    print(s.peek())
    print(s.size())
    print(s.isEmpty())

    s.push(5.2)
    print(s.pop())
    print(s.pop())
    print(s.size())

    print(parChecker("(a+1)*((c+d)/2)"))
    print(parChecker("(a+1)*((c+d/2)"))
    print(parChecker("{(a+1)}*((c+d)/2)"))
    print(parChecker("{(a+1)*(}(c+d)/2)"))

