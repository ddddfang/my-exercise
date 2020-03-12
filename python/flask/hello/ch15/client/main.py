from flask import request, jsonify
from taskInfo import *

# API 接口,接收任务请求
# http://127.0.0.1:8080/?name=trainTicket&taskId=1
@app.route('/')
def task_receive():
    taskId = request.args.get('taskId','')
    name = request.args.get('name', '')
    kwargs = {}
    kwargs['train_date'] = '2020-3-12'
    kwargs['from_station'] = '北京'
    kwargs['to_station'] = '上海'
    AutoTask.delay(taskId, name, **kwargs)  #AutoTask 会在执行任务结束时请求调度中心的
    return jsonify({'result' : 'success', 'taskId' : taskId})

#先启动celery任务池再启动网站
# -l info       等同于 --loglevel=info
# -P solo       使用单进程启动
# -P eventlet   使用协程方式启动,还需要安装 eventlet
# celery -A taskInfo.mycelery worker -l info -P solo

#所以总体顺序是: redis -> celery -> main.py

if __name__ == '__main__':
    app.run(port=8000, debug=True)
