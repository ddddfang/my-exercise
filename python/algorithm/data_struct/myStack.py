import string

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

# 2.将 decNumber 转化为任意进制的数(str),利用stack做逆序
def baseConvert(decNumber, base):
    digits = '0123456789abcdef'

    s = Stack()
    while decNumber > 0:
        rem = decNumber % base
        s.push(rem)
        decNumber = decNumber // base
    newStr = ''
    while not s.isEmpty():
        newStr += digits[s.pop()]
    return newStr

# 3.中序表达式和后序表达式的转换
# a + ( b + c ) * ( d - e ) --> abc+de-*+
# f * ( a + ( b + c ) * ( d - e ) ) --> fabc+de-*+*
def infix2postfix(infixExpr):
    prec = {}   #几个运算符的优先级存放在字典
    prec["*"] = 3
    prec["/"] = 3
    prec["+"] = 2
    prec["-"] = 2
    prec["("] = 1   #保证当(a+b)这种case, + 可以进栈
    
    opstack = Stack()
    postfixlist = []

    tokenlist = infixExpr.split()   #假设输入的表达式是以空格分隔

    for token in tokenlist:
        #if token in string.ascii_lowercase:
        if token.isdigit():
            postfixlist.append(token)
        elif token == '(':
            opstack.push(token)
        elif token == ')':
            toptoken = opstack.pop()
            while toptoken != '(':
                postfixlist.append(toptoken)
                toptoken = opstack.pop()
        else:
            #到这里的都是运算符了,当前优先级更高则需要进栈,否则一部分运算就可以确定先做
            while (not opstack.isEmpty()) and prec[opstack.peek()] >= prec[token]:
                postfixlist.append(opstack.pop())
            opstack.push(token)
    while not opstack.isEmpty():
        postfixlist.append(opstack.pop())
    return " ".join(postfixlist)

# 4.计算后序表达式的值
def doMath(op, operand1, operand2):
    if op == '*':
        return operand1 * operand2
    elif op == '/':
        return operand1 / operand2
    elif op == '+':
        return operand1 + operand2
    else:
        return operand1 - operand2
def postfixVal(postfixExpr):
    operandstack = Stack()

    tokenlist = postfixExpr.split()

    for token in tokenlist:
        if token.isdigit():
            operandstack.push(int(token))
        else:
            operand2 = operandstack.pop()
            operand1 = operandstack.pop()
            res = doMath(token, operand1, operand2)
            operandstack.push(res)
    return operandstack.pop()

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
    print(s)

    print(parChecker("(a+1)*((c+d)/2)"))
    print(parChecker("(a+1)*((c+d/2)"))
    print(parChecker("{(a+1)}*((c+d)/2)"))
    print(parChecker("{(a+1)*(}(c+d)/2)"))

    print(baseConvert(138, 16))

    #print(infix2postfix("a + ( b + c ) * ( d - e )"))
    #print(infix2postfix("f * ( a + ( b + c ) * ( d - e ) )"))

    post = infix2postfix("2 * ( 1 + ( 26 + 5 ) * ( 62 - 55 ) )")
    print(post)
    print(postfixVal(post))

    if '68' in '0123456789':
        print("78 in 0123456789")
    else:
        print("not in")
