from flask import Flask
from celery import Celery

#https://dormousehole.readthedocs.io/en/latest/
#据说这一块代码是flask官方网站上的?
def make_celery(app):
    mycelery = Celery(
        app.import_name,
        backend=app.config['CELERY_RESULT_BACKEND'],
        broker=app.config['CELERY_BROKER_URL']
    )
    mycelery.conf.update(app.config)

    class ContextTask(mycelery.Task):
        def __call__(self, *args, **kwargs):
            with app.app_context(): #在 app 的上下文中运行任务
                return self.run(*args, **kwargs)
    mycelery.Task = ContextTask       #这样后面就可以 celery.Task(xxx) 使用了
    return mycelery


app = Flask(__name__)

app.config.update(
    CELERY_BROKER_URL='redis://localhost:6379/0',
    CELERY_RESULT_BACKEND='redis://localhost:6379/1',
    JSON_AS_ASCII=False
)

mycelery = make_celery(app)

