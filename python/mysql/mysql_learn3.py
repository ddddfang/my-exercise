#ctags

import pymysql
import csv
import random
from functools import reduce
import os
import re

config = {
    "host" : "localhost",
    "port" : 3307,
    "user" : "root",
    "password" : "123456",
    "charset" : "utf8mb4",
    "database" : "test_db"
}
db = pymysql.connect(
    host = config["host"],
    port = config["port"],
    user = config["user"],
    password = config["password"],
    charset = config["charset"],
    db = config["database"] #这相当于 use database; 了
)

cursor = db.cursor()    # 获取操纵数据库的指针
table_name = "ctags"

cmd = '''
create table if not exists `{}` (
    `id` int unsigned not null auto_increment,
    `symbol_name` varchar(200) not null,
    `symbol_type` varchar(100) not null,
    `location` varchar(500) default null comment 'location->file:line',
    `content` text comment 'content',
    primary key (`id`)
) engine=InnoDB default charset={};
'''.format(table_name, config["charset"])
cursor.execute(cmd)



# generate ctags file.
project_root = "/home/fang/codes/zxnn_lib"
ctag_cmd = "".join(["ctags -R --fields=+n+m+K --c-kinds=+px ", project_root])
os.system(ctag_cmd)

with open("./tags", 'r') as fr:
    content = fr.readlines() 

    header = ['id','symbol_name','symbol_type','location','content']
    rows = []

    pa1 = re.compile(r'/\^.*;"|\d+;"', re.IGNORECASE)
    pa2 = re.compile(r'line:\d+', re.IGNORECASE)
    for i,line in enumerate(content[6:]):
        if i == 11236:
            print (line)
        line = line.replace("\t", " ")
        item = []
        item.append(i)

        linenumber = 0
        for target in pa1.findall(line):
            line = line.replace(target,"")
        for target in pa2.findall(line):
            line = line.replace(target,"")
            target = target.replace("line:","")
            linenumber = int(target)

        lineitems = line.split()
        symbol_name = lineitems[0]
        if symbol_name == "operator":
            symbol_name += lineitems[1]
            location = "".join([lineitems[2],":",str(linenumber)])
            symbol_type = lineitems[3]
        else:
            location = "".join([lineitems[1],":",str(linenumber)])
            symbol_type = lineitems[2]
    
        # 插入到 db
        cmd = '''
            insert into {} (`symbol_name`, `symbol_type`, `location`) 
            values("{}", "{}", "{}")
        '''.format(table_name, symbol_name, symbol_type, location)
        cursor.execute(cmd)

        item.append(symbol_name)
        item.append(symbol_type)
        item.append(location)
        rows.append(item)
        #print("{} {} {}".format(symbol_name, symbol_type, location))
    db.commit() # 想让 insert 生效 必须加上 commit

    # write result to csv file.
    with open("./res.csv", 'w') as fw:
        f_csv = csv.writer(fw)
        f_csv.writerow(header)
        #rows = [
        #    [1,'xiaoming','male',168,23],
        #    [1,'xiaohong','female',162,22],
        #    [2,'xiaozhang','female',163,21],
        #    [2,'xiaoli','male',158,21]
        #]
        f_csv.writerows(rows)

db.close()
