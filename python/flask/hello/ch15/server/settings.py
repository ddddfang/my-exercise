#conda install flask,flask-admin,flask-sqlalchemy,flask-babel
#redis-server
#sudo docker run --name first-mysql -p 3307:3306 -e MYSQL_ROOT_PASSWORD=123456 -d mysql

from flask import Flask,request,jsonify,render_template
from flask_sqlalchemy import SQLAlchemy
from flask_admin import Admin
from flask_babel import Babel

app = Flask(__name__)

#使用Babel翻译扩展工具本地化,将网页改为中文显示
#这里定义一个Babel对象
babel = Babel(app)

#启动mysql server docker后, mysql -h127.0.0.1 -P3307 -uroot -p123456
#设置系统连接的数据库为mysql,委托 SQLAlchemy 使用 pymysql 进行数据库的连接
#用户名 root,密码 123456,数据库名字 automation, charset为 UTF8MB4
URI = 'mysql+pymysql://root:123456@127.0.0.1:3307/automation?charset=UTF8MB4'

app.config.update(
    SQLALCHEMY_DATABASE_URI=URI,    #设置此选项后,Flask将使用 SQLAlchemy 作为 ORM 框架
    SQLALCHEMY_TRACK_MODIFICATIONS=True,    #设置为True或False都可以避免warning
    BABEL_DEFAULT_LOCALE='zh_CN',   #设置此选项后,Flask将使用 Babel 将网页翻译为中文
    SECRET_KEY='213sd456s51',       #设置密钥值,这是随便写的
    JSON_AS_ASCII=False
)

#将 Flask 与 SQLAlchemy 绑定
db = SQLAlchemy(app)

#name定义admin后台的名字,template_mode设置网页样式
admin = Admin(app, name='任务调度中心', template_mode='bootstrap3')


