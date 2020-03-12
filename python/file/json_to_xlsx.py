import json
import glob
#import os
#import csv
import pandas as pd
import time

files = glob.glob("./abc.json")
headers = ['id','weiboId','uid','forwarderId','published','content','level']

for each_file in files:
    print(each_file)
    rows = []
    with open(each_file, encoding='utf-8') as fr:
        content = fr.read()
        dict_all = json.loads(content)
        #print(dict_all)

        #print(dict_all["code"])
        #print(dict_all["extendStats"])
        #print(dict_all["message"])
        #print(dict_all["serverInfo"])

        for iContentCommonNet in dict_all['extendStats']['iContentCommonNetLists']:
            for item in iContentCommonNet['iContentCommonNetList']:
                row = []
                row.append(item['id'].split('-')[0])
                row.append(item['weiboId'])
                row.append(item['uid'])
                row.append(item['forwarderId'])

                timeStamp = int(item['published']/1000)
                timeArray = time.localtime(timeStamp)
                otherStyleTime = time.strftime("%Y-%m-%d %H:%M:%S", timeArray)
                row.append(otherStyleTime)
                
                row.append(item['content'])
                row.append(item['level'])
                print(row)
                rows.append(row)

    df = pd.DataFrame(rows, columns=headers)
    df.to_excel(each_file.replace(".json",".xlsx"))



