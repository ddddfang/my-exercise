import requests

def city_name():
    url = 'https://kyfw.12306.cn/otn/resources/js/framework/station_name.js?station_version=1.9063'
    city_code = requests.get(url)
    city_code_list = city_code.text.split("|")
    city_dict = {}
    for k, i in enumerate(city_code_list):
        if '@' in i:
            # 城市名作为字典的键，城市编号作为字典的值
            city_dict[city_code_list[k + 1]] = city_code_list[k + 2]
    return city_dict

def get_info(train_date, from_station, to_station):
    # 将城市名转换成城市编号
    city_dict = city_name()
    from_station = city_dict[from_station]
    to_station = city_dict[to_station]
    # 发送请求
    params = {
        'leftTicketDTO.train_date': train_date,
        'leftTicketDTO.from_station': from_station,
        'leftTicketDTO.to_station': to_station,
        'purpose_codes': 'ADULT'
    }
    headers = {
        'User-Agent' : 'Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:65.0) Gecko/20100101 Firefox/65.0',
        'Host' : 'kyfw.12306.cn'
    }
    info_list = []
    # 通过try……except方式分别对不同的URL进行访问
    try:
        url = 'https://kyfw.12306.cn/otn/leftTicket/query'
        r = requests.get(url, headers=headers, params=params)
        #url = 'https://kyfw.12306.cn/otn/leftTicket/query?leftTicketDTO.train_date=2020-03-19&leftTicketDTO.from_station=SHH&leftTicketDTO.to_station=JNK&purpose_codes=ADULT'
        #r = requests.get(url, headers=headers)
        info_text = r.json()['data']['result']
        # 获取响应内容并提取有效数据
        for i in info_text:
            info_dict = {}
            train_info = i.split('|')
            info_dict['train_no'] = train_info[3]
            info_dict['start_time'] = train_info[8]
            info_dict['end_time'] = train_info[9]
            info_dict['interval_time'] = train_info[10]
            info_dict['second_seat'] = train_info[30]
            info_dict['frist_seat'] = train_info[31]
            info_dict['special_seat'] = train_info[32]
            info_list.append(info_dict)
    except:
        print('er...')
    return info_list

if __name__ == '__main__':
    train_date = '2020-03-19'
    from_station = '广州'
    to_station = '武汉'
    info = get_info(train_date, from_station, to_station)
    print(str(info))
