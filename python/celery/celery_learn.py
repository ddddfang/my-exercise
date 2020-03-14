from celery import Celery

mycelery = Celery('tasks', broker='redis://localhost:6379/0', backend='redis://localhost:6379/1')

#class ContextTask(mycelery.Task):
#    def __call__(self, *args, **kwargs):
#        return self.run(*args, **kwargs)
#mycelery.Task = ContextTask #这样后面使用 @mycelery.task() 装饰器的函数就是关联到这个 ContextTask

@mycelery.task
def task_add(x, y):
    print ("task_add action in ...")
    return x + y

# 1.启动 redis-server
# 2.terminal 启动celery任务池
#   -A 指定应用中的celer对象,即 celery_learn.py 中的 mycelery
#   -l info       等同于 --loglevel=info
#   -P solo       使用单进程启动
#   -P eventlet   使用协程方式启动,还需要安装 eventlet
# celery worker -A celery_learn.mycelery -l info -P solo

# 3.发布任务,写在代码中或 进入 python>>> 环境
# from celery_learn import task_add
# task_add.delay(3,6)
# task_add.apply_async(args=[3, 6])
# task_add.apply_async(args=[3, 6], countdown=60)

# 3.或者 直接 python celery_learn.py worker -c 2 
# -c 2 表示启动任务池中 worker 的个数,默认为内核个数
#if __name__ == "__main__":
#    mycelery.start()

# 4.至于结果的返回,要么 task_add() 中通过某种方式写到某处
# 要么 result = task_add.delay(),然后 发现 result.ready() 为 True 后,
# result.get()
