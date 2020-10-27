#hash表,python提供字典就是最好的实现,下面只是给出一个例子

class myHash:
    def __init__(self):
        self.size = 11
        self.keys = [None] * self.size
        self.vals = [None] * self.size
    def put(self, key, val):
        #
        None
    def get(self, key):
        return 0
    def __getitem__(self, key):     #这样就可以通过[]访问我们的这个 myHash 类型了
        return 0
    def __setitem__(self, key, item):
        #
        None
