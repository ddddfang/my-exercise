#导入我们的 settings.py 中定义的对象
from settings import *
from datetime import datetime

#一个继承自 db.Model 的类,SQLAlchemy.Model 类与数据库实现了映射定义
#程序基本信息表,会映射到 pymysql 的定义table
class ProgramInfo(db.Model):
    __tablename__ = 'ProgramInfo'
    id = db.Column(db.INT, primary_key=True)
    clientIP = db.Column(db.String(50))
    introduce = db.Column(db.String(50))
    name = db.Column(db.String(50), unique=True)
    statusLock = db.Column(db.String(50))

#定义任务记录表
class TaskRecord(db.Model):
    __tablename__ = 'TaskRecord'
    id = db.Column(db.INT, primary_key=True)
    clientIP = db.Column(db.String(50))
    name = db.Column(db.String(50))
    statue = db.Column(db.String(50))
    #createTime = db.Column(db.DateTime, server_default=db.func().now())
    createTime = db.Column(db.DateTime, default=datetime.now)


#触发 SQLAlchemy 调用 pymysql 创建 table
#创建哪些table? 应该是所有继承自 db.Model 的那些类
db.create_all()


#插入数据
p = ProgramInfo()
p.clientIP = 'localhost'
p.introduce = 'Auto Command Executor'
p.name = 'Machine.01'
p.statusLock = ''
db.session.add(p)
db.session.commit()
for i in ProgramInfo.query.all():   #返回 ProgramInfo 类组成的 list
    print("{}:{}".format(type(i), i))
print("--------------------insert complete---------------------------")

#更新数据
name = 'Machine.01'
qs = ProgramInfo.query.filter_by(name=name)
qs.update({ProgramInfo.introduce : 'Command Executor Machines'})
print(ProgramInfo.query.all())
print("--------------------update complete---------------------------")

#查询数据
print(ProgramInfo.query.all())
#条件查询
print(ProgramInfo.query.filter_by(name='Machine.01'))
#查询某字段
print(db.session.query(ProgramInfo.name).all())
print("------------------get complete-----------------------------")

#删除数据
#qs = db.session.query().filter_by(name='Machine.01').first()
qs = ProgramInfo.query.filter_by(name='Machine.01').first()
db.session.delete(qs)
db.session.commit()
print("-------------------delete complete----------------------------")







