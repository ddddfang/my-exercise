#导入我们的 models.py 中定义的类
from models import *
from flask_admin.contrib.sqla import ModelView
from flask_admin.actions import action
import requests


#定义模型 ProgramInfo 对应的后台
class ProgramInfoAdmin(ModelView):
    #设置字段中文内容
    column_labels = dict(clientIP='IP地址', introduce='描述', name='名称', statusLock='状态锁')

    #设置每页的数据量
    page_size = 30

    @action('执行任务', '执行任务', '确定执行任务?')
    def action_task(self, ids):
        for id in ids:
            info = ProgramInfo.query.filter_by(id=id).first()
            if not info.statusLock:
                ip = info.clientIP
                name = info.name

                #写入任务记录表
                data = TaskRecord(clientIP=ip, name=name)
                db.session.add(data)

                #获取刚刚写入的数据的主键
                db.session.flush()
                taskId = str(data.id)

                url = ip + '?name=' + name + '&taskId=' + taskId
                print(url)

                try:
                    r = requests.get(url)
                    if r.status_code == 200:
                        info.statusLock = 'Lock'
                except:
                    pass

                #保存到数据库
                db.session.commit()

#在 admin 注册视图
admin.add_view(ProgramInfoAdmin(ProgramInfo, db.session, name='程序信息表'))

#定义模型 TaskRecord 对应的后台
class TaskRecordAdmin(ModelView):
    column_labels = dict(clientIP='IP地址', name='名称', createTime='创建时间')

#在 admin 注册视图
admin.add_view(TaskRecordAdmin(TaskRecord, db.session, name='任务记录表'))


