# python 的 pymysql 或者其他connector只是mysql的 client.
# mysql 服务器是需要单独在 ubuntu 上安装的.我直接使用的 docker:
# sudo docker run --name first-mysql -p 3307:3306 -e MYSQL_ROOT_PASSWORD=123456 -d mysql 
# 然后在 localhost:3307 应该就有 sql 服务了.

# 方便起见:可以在 ubuntu 上安装 mysql-client: sudo apt-get install mysql-client
# 然后在确保 docker(mysql) 运行的前提下: mysql -h127.0.0.1 -P3307 -uroot -p123456 即可登录
# mysql的命令行客户端.


# 关于pymysql基本数据的测试: https://github.com/PyMySQL/PyMySQL/blob/master/pymysql/tests/test_basic.py
# https://www.cnblogs.com/leijiangtao/p/11874182.html

# 这个脚本: 读取excel并将其中的内容写入 test_db.table_zx 中
# 完成后 从数据库中检索数据,作为数据库操作的基本sample

import pymysql
import glob
import pandas as pd

config = {
    "host" : "localhost",
    "port" : 3307,
    "user" : "root",
    "password" : "123456",
    "charset" : "utf8mb4",
    "database" : "test_db"
}

#=============== connect and create database =========================
db = pymysql.connect(
    host = config["host"],
    port = config["port"],
    user = config["user"],
    password = config["password"],
    charset = config["charset"]
)

cursor = db.cursor()
cmd = "create database if not exists {}".format(config["database"])
cursor.execute(cmd)
db.close()

#================= connect and open database  =======================
db = pymysql.connect(
    host = config["host"],
    port = config["port"],
    user = config["user"],
    password = config["password"],
    charset = config["charset"],
    db = config["database"] #这相当于 use database; 了
)

cursor = db.cursor()    # 获取操纵数据库的指针
#db.select_db('gloryroad') # 不加这句话默认就是 test_db 数据库,加上的话就是选择其他数据库 use gloryroad;

#================= drop and re-create table in database  ===============
table_name = "table_zx"
table_name2 = "usr_info"

# drop table1 and table2
cmd = "drop table if exists `{}`".format(table_name)
cursor.execute(cmd)
db.commit() # 想让 insert 生效 必须加上 commit

cmd = "drop table if exists `{}`".format(table_name2)
cursor.execute(cmd)
db.commit() # 想让 insert 生效 必须加上 commit

# create table
cmd = '''
create table if not exists `{}` (
   `id` int unsigned not null auto_increment,
   `product_name` char(40) not null,
   `product_id` varchar(20) not null,
   `number` int default 1,
   `owner` varchar(40) not null,
   primary key (`id`)
) engine=InnoDB default charset={};
'''.format(table_name, config["charset"])
cursor.execute(cmd)

cmd = '''
create table if not exists `{}` (
    `id` int unsigned not null auto_increment comment '主键',
    `name` varchar(20) default null comment '姓名',
    `mobile` char(11) default null comment '手机号码',
    `address` varchar(100) default null comment '住址',
    `description` text comment '个人简介',
    `sex` char(1) default null comment '性别',
    `age` tinyint(1) default null comment '年龄',
    `idno` char(18) default null comment '身份证号码',
    `image` blob comment '照片',
    `recordtime` datetime comment '记录时间',
    primary key (`id`),
    unique key `person` (`mobile`),
    unique key `idcard` (`idno`),
    key `telephone` (`mobile`)
) engine=InnoDB auto_increment=2 default charset={};
'''.format(table_name2, config["charset"])
cursor.execute(cmd)

files = glob.glob("/home/fang/桌面/*.xlsx")
print(files)

#read
with pd.ExcelFile(files[0]) as ef: # 带有解析操作
    for sh in ef.sheet_names:
        df = pd.read_excel(ef, sh)
        #data = df           # 所有数据, pandas.core.frame.DataFrame 格式
        data = df.values    # 返回一个 numpy.ndarray
        #print("excel data({0}){1} :{2}.".format(type(data), data.shape, data))

        #使用单行插入的方式
        lines = df.index.values
        #print(lines)
        for line in lines:
            d = df.iloc[line, :].values
            #print("line{} is {},{},{},{}".format(line, d[0], d[1], d[2], d[3]))

            ##方式1
            #cmd = '''
            #    insert into {} (`product_name`, `product_id`, `number`, `owner`) 
            #    values("{}", "{}", "{}", "{}")
            #'''.format(table_name, d[0], d[1], d[2], d[3])
            ##print(cmd)
            #cursor.execute(cmd)

            #方式2
            cmd = "insert into {} (`product_name`,`product_id`,`number`,`owner`) values (%s, %s, %s, %s)".format(table_name)   #sql会自动给 %s 字段加 ''
            v = tuple(map(str,list(d)))    #将d(ndarray)搞成转为list,然后吧list中每一个值转成str,再组织成tuple
            cursor.execute(cmd, v)

            #print(cursor.lastrowid)

            #if line > 10:
            #    break

        ##直接将多行数据转换为 list[list] 然后插入
        #cmd = "insert into {} (`product_name`,`product_id`,`number`,`owner`) values (%s, %s, %s, %s)".format(table_name)   #sql会自动给 %s 字段加 ''
        ##v = [ 
        ##    ('x','y','z','w') #这里是元祖或list都是可以的
        ##    ('x','y','z','w')
        ##    ('x','y','z','w')
        ##    ('x','y','z','w')
        ##    ('x','y','z','w')
        ##]
        #list_lines = list(data)    # 二维ndarray强制转换为list组成的list
        ##print(list_lines)
        #v = [ tuple(map(str,i)) for i in list_lines]  # 对每一个内层 list 做 tuple(map(str,list(d))) 处理
        #cursor.executemany(cmd,v)
        db.commit() # 想让 insert 生效 必须加上 commit

#cursor.execute("SELECT VERSION()")
#data = cursor.fetchone()
#print ("Database version : {}".format(data))


cmd = "select * from {} order by id limit 30".format(table_name)
cursor.execute(cmd)

data = cursor.fetchone()    # 返回元组
print(type(data), data)

data = cursor.fetchmany(3)  # 返回元组的元组
print(type(data), data)

cursor.scroll(1, mode="absolute")   #光标定位到 offset=1 处
cursor.scroll(-1, mode="relative")   #光标向下(正数)偏移,或向上(负数)偏移
data = cursor.fetchall()    # 返回元组的元组
print(type(data), data)

cursorDict = db.cursor(cursor=pymysql.cursors.DictCursor)   #获取操纵数据库的指针,插入和查询应该都是字典类型
cursorDict.execute(cmd)
data = cursorDict.fetchmany(3)  # 返回字段组成的列表
print(type(data), data)


cursorDict.close()
cursor.close()
db.close()




if __name__ == "__main__":
    pass
