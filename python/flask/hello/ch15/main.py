from admin import *
from flask import request, jsonify

#API接口, 用于接收 client 的运行结果
# 127.0.0.1:8080/?taskId=3&name=test&status=Done
@app.route('/')
def callback():
    taskId = request.args.get('taskId', '')
    name = request.args.get('name', '')
    status = request.args.get('status', '')

    if taskId and name:
        #在任务记录表修改任务执行状态
        task = TaskRecord.query.filter_by(id=int(taskId)).first()
        task.status = status

        #在任务信息表释放任务的状态锁
        info = ProgramInfo.query.filter_by(name=name).first()
        info.statusLock = ''

        db.session.commit()
        #返回响应内容
        response = {
            'result' : 'success'
        }
        return jsonify(response)

    else:
        response = {
            'result' : 'fail'
        }
        return jsonify(response)

if __name__ == '__main__':
    app.run(port=8080, debug=True)


