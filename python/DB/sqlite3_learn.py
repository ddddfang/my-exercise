# create connection
# get cursor
# execute sql
# close cursor
# close connection

import sqlite3
print(dir(sqlite3.Connection.cursor))

conn = sqlite3.connect('mrsoft.db')
cursor = conn.cursor()

#删除已经存在的表
cursor.execute("drop table if exists user")

#初次创建table后,后面就不要再创建了
sql_str = '''
create table user(
    id int(10) primary key,
    name varchar(20))
'''
cursor.execute(sql_str)

#向表中插入数据
cursor.execute("insert into user (id, name) values ('1', 'white')")
cursor.execute("insert into user (id, name) values ('2', 'josy')")
cursor.execute("insert into user (id, name) values ('3', 'anddy')")
cursor.execute("insert into user (id, name) values ('4', 'yann')")
cursor.execute("insert into user (id, name) values ('5', 'jensen')")
cursor.execute("insert into user (id, name) values ('6', 'fabio')")

#更新表中的数据
cursor.execute("update user set name=? where id=?",('black',2))

#删除表中的数据
cursor.execute("delete from user where id=?",(1,))

#查询表中的数据
#cursor.execute("select * from user")
cursor.execute("select * from user where id > ?",(1,)) #使用?占位符,元组元素为1时候,不可以省略
result1 = cursor.fetchone()     #每次fetch后,cursor都会自动下移
print(result1)
result2 = cursor.fetchmany(2)   #获得两条查询结果(组成的list)
print(result2)
result3 = cursor.fetchall()     #获得所有查询结果(组成的list)
print(result3)

cursor.close()
#对于 增加 修改 删除操作,使用commit提交事务后,如果操作失败可以使用rollback()回滚到操作前
conn.commit()
conn.close()


