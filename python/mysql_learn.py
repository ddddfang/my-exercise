# python 的 pymysql 或者其他connector只是mysql的 client.
# mysql 服务器是需要单独在 ubuntu 上安装的.我直接使用的 docker:
# sudo docker run --name first-mysql -p 3307:3306 -e MYSQL_ROOT_PASSWORD=123456 -d mysql 
# 然后在 localhost:3307 应该就有 sql 服务了.

# 方便起见:可以在 ubuntu 上安装 mysql-client: sudo apt-get install mysql-client
# 然后在确保 docker(mysql) 运行的前提下: mysql -h127.0.0.1 -P3307 -uroot -p123456 即可登录
# mysql的命令行客户端.

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
    db = config["database"]
)

cursor = db.cursor()
cmd = '''
create table if not exists `{}` (
   `id` int unsigned not null auto_increment,
   `product_name` char(40) not null,
   `product_id` int not null,
   `number` int default 1,
   `owner` varchar(40) not null,
   primary key (`id`)
) engine=InnoDB default charset={};
'''.format("table_zx",config["charset"])
cursor.execute(cmd)

cursor.execute("SELECT VERSION()")
data = cursor.fetchone()
print ("Database version : {}".format(data))



cursor.close()
db.close()



def demo1():    # read and write xlsx
    files = glob.glob("/home/fang/桌面/*.xlsx")
    print(files)

    #read
    with pd.ExcelFile(files[0]) as ef: # 带有解析操作
        for sh in ef.sheet_names:
            df = pd.read_excel(ef, sh)
            #data = df           # 所有数据, pandas.core.frame.DataFrame 格式
            data = df.values    # 返回一个 numpy.ndarray
            print("excel data({0}){1} :{2}.".format(type(data), data.shape, data))

            lines = df.index.values
            print(lines)

            ##data = df.ix[10].values    #读取第10行的内容
            ##data = df.iloc[10:12].values    #读取第[10,12)行内容
            #data = df.iloc[10:12, 0:2].values    #读取第[10,12)行,[0,2)列内容
            #print("line10 is {}{}".format(type(data), data))


if __name__ == "__main__":
    demo1()
