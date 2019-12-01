# python 的 pymysql 或者其他connector只是mysql的 client.
# mysql 服务器是需要单独在 ubuntu 上安装的.我直接使用的 docker:
# sudo docker run --name first-mysql -p 3307:3306 -e MYSQL_ROOT_PASSWORD=123456 -d mysql 
# 然后在 localhost:3307 应该就有 sql 服务了.

import pymysql

#db = pymysql.connect(host='localhost', port=3307, user="root", password="123456", charset="utf8mb4")
#cursor = db.cursor()
#cursor.execute("create database if not exists test_db")
#db.close()

db = pymysql.connect(host='localhost', port=3307, user="root", password="123456", charset="utf8mb4", db="test_db")
cursor = db.cursor()
cursor.execute("SELECT VERSION()")
data = cursor.fetchone()
print ("Database version : {}".format(data))
db.close()
