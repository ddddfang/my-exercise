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
    db = config["database"] #这相当于 use database; 了
)

cursor = db.cursor()

table_name = "table_zx"
# drop table
cmd = "drop table if exists {}".format(table_name)
cursor.execute(cmd)

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


files = glob.glob("/home/fang/桌面/*.xlsx")
print(files)

#read
with pd.ExcelFile(files[0]) as ef: # 带有解析操作
    for sh in ef.sheet_names:
        df = pd.read_excel(ef, sh)
        #data = df           # 所有数据, pandas.core.frame.DataFrame 格式
        data = df.values    # 返回一个 numpy.ndarray
        #print("excel data({0}){1} :{2}.".format(type(data), data.shape, data))

        lines = df.index.values
        #print(lines)
        for line in lines:
            d = df.iloc[line, :].values
            #print("line{} is {},{},{},{}".format(line, d[0], d[1], d[2], d[3]))

            cmd = '''
                insert into {} (`product_name`, `product_id`, `number`, `owner`) 
                values("{}", "{}", "{}", "{}")
            '''.format(table_name, d[0], d[1], d[2], d[3])
            #print(cmd)
            cursor.execute(cmd)

            # error example ......
            #cmd = "insert into {} (%s) values(%s)".format(table_name)   #sql会自动给 %s 字段加 ''
            ##print(cmd)
            #header = ",".join(["`product_name`", "`product_id`", "`number`", "`owner`"])
            #row = ",".join(['"{}"'.format(d[0]), '"{}"'.format(d[1]), '"{}"'.format(d[2]), '"{}"'.format(d[3])])
            #print(header)
            #print(row)
            #cursor.execute(cmd, [header, row])

            #print(cursor.lastrowid)

            #if line > 10:
            #    break
        db.commit() # 想让 insert 生效 必须加上 commit

#cursor.execute("SELECT VERSION()")
#data = cursor.fetchone()
#print ("Database version : {}".format(data))

cmd = "select * from {} order by owner limit 50".format(table_name)
cursor.execute(cmd)

data = cursor.fetchone()    # 返回元组
print(type(data), data)

data = cursor.fetchmany(3)  # 返回元组的元组
print(type(data), data)

data = cursor.fetchall()    # 返回元组的元组
print(type(data), data)


cursor.close()
db.close()




if __name__ == "__main__":
    pass
