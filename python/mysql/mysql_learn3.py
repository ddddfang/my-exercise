#ctags

import pymysql
import csv
import random
from functools import reduce
import os
import re

#config = {
#    "host" : "localhost",
#    "port" : 3307,
#    "user" : "root",
#    "password" : "123456",
#    "charset" : "utf8mb4",
#    "database" : "test_db"
#}
#db = pymysql.connect(
#    host = config["host"],
#    port = config["port"],
#    user = config["user"],
#    password = config["password"],
#    charset = config["charset"],
#    db = config["database"] #这相当于 use database; 了
#)
#
#cursor = db.cursor()    # 获取操纵数据库的指针
#table_name = "ctags"
#
#cmd = '''
#create table if not exists `{}` (
#    `id` int unsigned not null auto_increment,
#    `symbol_name` varchar(40) not null,
#    `definition` varchar(100) default null comment 'definition->file:line',
#    `declaration` varchar(100) default null comment 'declaration->file:line',
#    `content` text comment 'content',
#    primary key (`id`)
#) engine=InnoDB default charset={};
#'''.format(table_name, config["charset"])
#cursor.execute(cmd)

project_root = "/home/fang/codes/zxnn_lib"
ctag_cmd = "".join(["ctags -R --fields=+n+m+K --c-kinds=+px ", project_root])
os.system(ctag_cmd)

with open("./tags", 'r') as fr:
    content = fr.readlines() 

    header = ['id','symbol_name','type','definition','content']
    rows = []

    pa1 = re.compile(r'/\^.*;"|\d+;"', re.IGNORECASE)
    pa2 = re.compile(r'line:\d+', re.IGNORECASE)
    for i,line in enumerate(content[6:]):
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
        definition = "".join([lineitems[1],":",str(linenumber)])
        symbol_type = lineitems[2]

        item.append(symbol_name)
        item.append(symbol_type)
        item.append(definition)
        rows.append(item)
        print("{} {} {}".format(symbol_name, symbol_type, definition))

    with open("./res.csv", 'w') as fw:
        f_csv = csv.writer(fw)
        f_csv.writerow(header)
        f_csv.writerows(rows)
        #rows = [
        #    [1,'xiaoming','male',168,23],
        #    [1,'xiaohong','female',162,22],
        #    [2,'xiaozhang','female',163,21],
        #    [2,'xiaoli','male',158,21]
        #]

