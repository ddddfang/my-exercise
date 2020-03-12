from settings import *
import requests
from trainTicket import *

@mycelery.task()
def AutoTask(taskId, name, **kwargs):
    try:
        if name == 'printPDF':
            print('printPDF')
        elif name == 'trainTicket':
            train_date = kwargs['train_date']
            from_station = kwargs['from_station']
            to_station = kwargs['to_station']
            i = get_info(train_date, from_station, to_station)
            print(str(i))
        status = 'Done'
    except Exception as e:
        print(e)
        status = 'Fail'

    #将运行结果返回调度中心
    url = 'http://127.0.0.1:8080/?name=' + name + '&taskId=' + taskId + '&status=' + status

    try:
        requests.get(url)
    except:
        pass
    return status



