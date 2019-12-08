
# 这个脚本: 获取 test_db.table_zx 中的 owner 信息,并造一份 usr_info 信息数据导出到csv文件

import pymysql
import csv
import random
from functools import reduce


def get_random_name():
    last_name = [   "宋","李","张","唐","谭","高",
                    "王","汪","石","秦","白","袁",
                    "贾","孙","史","展","区","步",
                    "欧阳","方","周","翁","施"]
    first_name = [  "爱才","文国","卫国","爱国","真","天一",
                    "怡","芳","婷","玉","蒙","圆",
                    "凯奇","立","元芳","德","思古","三清",
                    "程","地和","感","丁","廷玉",
                    "友德","君","文君","军","玲","安"]
    return ''.join([random.choice(last_name), random.choice(first_name)])

def get_random_mobile():
    mobile_suffix = ''.join(str(random.choice(range(10))) for _ in range(8))
    mobile_prefix = random.choice([131,132,133,158,134,135,136,137,138,139,181,176,185])
    mobile = "{}{}".format(mobile_prefix, mobile_suffix)
    return mobile

def get_random_address():
    city_list = ["北京市", "上海市", "天津市", "重庆市", "江苏省南京市", "黑龙江省大庆市", "甘肃省庆阳市", "安徽省安庆市", "江苏省徐州市"]
    road_list = ["人民路", "光华路", "朝阳路", "南京路", "宣武路", "解放路"]
    city = random.choice(city_list)
    road = random.choice(road_list)
    street = ''.join(str(random.choice(range(10))) for _ in range(3))
    number = ''.join(str(random.choice(range(10))) for _ in range(3))
    return ' '.join([city, road, street, "街道", number, "号"])



config = {
    "host" : "localhost",
    "port" : 3307,
    "user" : "root",
    "password" : "123456",
    "charset" : "utf8mb4",
    "database" : "test_db"
}

#================= connect and open database  =======================
db = pymysql.connect(
    host = config["host"],
    port = config["port"],
    user = config["user"],
    password = config["password"],
    charset = config["charset"],
    db = config["database"] #这相当于 use database; 了
)

table_name = "table_zx"
cmd = "select distinct owner from table_zx".format(table_name)

cursor = db.cursor()    # 获取操纵数据库的指针
cursor.execute(cmd)
data = cursor.fetchall()    # 返回元组的元组
#print(type(data), data)

#cursorDict = db.cursor(cursor=pymysql.cursors.DictCursor)   #获取操纵数据库的指针,插入和查询应该都是字典类型
#cursorDict.execute(cmd)
#data = cursorDict.fetchall()  # 返回字段组成的列表
#print(type(data), data)
#cursorDict.close()

for i in data:
    name = i[0]
    mobile = get_random_mobile()
    address = get_random_address()
    parent = get_random_name()
    print("name={},mobile={},{},addr={}".format(i[0], mobile, parent, address))

#cmd = '''
#create table if not exists `{}` (
#    `id` int unsigned not null auto_increment comment '主键',
#    `name` varchar(20) default null comment '姓名',
#    `mobile` char(11) default null comment '手机号码',
#    `address` text comment '住址',
#    `sex` char(1) default null comment '性别',
#    `age` tinyint(1) default null comment '年龄',
#    `idno` char(18) default null comment '身份证号码',
#    `image` blob comment '照片',
#    `recordtime` datetime comment '记录时间',
#    primary key (`id`),
#    unique key `person` (`mobile`),
#    unique key `idcard` (`idno`),
#    key `telephone` (`mobile`)
#) engine=InnoDB auto_increment=2 default charset={};
#'''.format(table_name2, config["charset"])





with open("./test.csv", 'w') as f:
    f_csv = csv.writer(f)
    header = ['class','name','sex','height','year']
    rows = [
        [1,'xiaoming','male',168,23],
        [1,'xiaohong','female',162,22],
        [2,'xiaozhang','female',163,21],
        [2,'xiaoli','male',158,21]
    ]
    f_csv.writerow(header)
    f_csv.writerows(rows)


cursor.close()
db.close()




if __name__ == "__main__":
    pass
