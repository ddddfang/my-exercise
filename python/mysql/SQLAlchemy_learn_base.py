#https://www.liaoxuefeng.com/wiki/897692888725344/955081460091040
#https://www.cnblogs.com/Zzbj/p/10212279.html

#conda install sqlalchemy #发现在flask学习的时候,安装 flask_sqlalchemy
#时就已经默认将 sqlalchemy 一并安装了.

#ORM 的初衷就是:使用python DB-API(eg.pymysql or mysql.connector)插入和查询都是list/dict 方式很不直观
#如果插入以类的形式,查询出来就是类的列表就很好,ORM就是干这个的

from sqlalchemy import Column, String, create_engine
from sqlalchemy.orm import sessionmaker
from sqlalchemy.ext.declarative import declarative_base

# 创建对象的基类:
Base = declarative_base()

# 定义User对象:
class User(Base):
    # 表的名字:
    __tablename__ = 'user'

    # 表的结构:
    id = Column(String(20), primary_key=True)
    name = Column(String(20))

# 初始化数据库连接: '数据库类型+数据库驱动名称://用户名:口令@机器地址:端口号/数据库名'
URI = 'mysql+pymysql://root:123456@127.0.0.1:3307/test_db?charset=UTF8MB4'
engine = create_engine(URI)

Base.metadata.drop_all(engine)
Base.metadata.create_all(engine)

# 创建DBSession类型:
DBSession = sessionmaker(bind=engine)

#向数据库中添加一条记录
# 创建session对象:
session = DBSession()
# 创建新User对象:
new_user = User(id='5', name='Bob')
# 添加到session:
session.add(new_user)
session.add_all([
    User(id='0', name='black'),
    User(id='1', name='joey'),
    User(id='2', name='billy'),
    User(id='3', name='charley')
])
# 提交即保存到数据库:
session.commit()
# 关闭session:
session.close()


# 创建Session:
session = DBSession()

# 创建Query查询，filter是where条件，最后调用one()返回唯一行，如果调用all()则返回所有行:
#user = session.query(User).filter(User.id=='5').one()
users = session.query(User).all()

print("type {},{}".format(type(users), users))
for user in users:
    print("user's id {},name {}.".format(user.id, user.name))
    
# 关闭Session:
session.close()



