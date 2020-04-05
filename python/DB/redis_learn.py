#安装 redis,就是参照 codes/cpp-js-go-reids-nginx/Redis入门指南（第2版）.pdf 所说安装
#深入学习 redis ---> Redis入门指南（第2版）.pdf


#首先去 https://redis.io/download 下载最新 .tar.gz 
#解压到 codes/redis/ 目录,然后 make & sudo make install

#然后 redis-server --port=6379 (6379 为默认port,所以--port可以省略) 启动 redis 服务
#然后 redis-cli 就可以操作 redis 了

#通过python操作redis:
#conda install redis (事实证明这个并不能操作python)
#conda install redis-py (这个才是python操作redis的模块)


import redis


r = redis.Redis(host='localhost', port=6379, decode_responses=True)
r.set('name', 'fang')
print(r['name'])
print(r.get('name'))
print(type(r.get('name')))


