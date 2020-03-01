from flask import Flask
from flask import request
from flask import jsonify
from flask import render_template

# 创建一个 Flask 对象实例,代表整改网站系统
app = Flask(__name__)



# route装饰器(装饰下面的hello_world函数,python语法?) 设置路由地址,即网页地址,url
@app.route('/')
# 此url的处理函数,可以返回str,json,或html
def hello():
    # 返回的网页
    return "this is hello page.."

# url = 127.0.0.1:5000/json/
@app.route('/json/')
def helloJson():
    ret = {
        'name' : 'fang',
        'passwd' : '12345',
        'score' : 92,
        'response' : 'Json'
    }
    return jsonify(ret)

# url = 127.0.0.1:5000/html
@app.route('/html/')
def helloHtml():
    #自己生成源码的话,使用jinja模板可能更方便一些
    title = 'fang.'
    user = {
        'nickname': 'ddddfang'
    } 
    # render_template 要求html文件都要放在运行文件的当前目录下的一个叫 templates/ 文件夹下
    # title 和 user 都是 index2.html 中使用到的变量
    return render_template('index2.html', title=title, user=user)

    ##直接返回html源码
    #with open('./templates/index.html') as fr:
    #    html = fr.read()
    #    return html

# url = 127.0.0.1:5000/user/
@app.route('/user/', methods=['GET', 'POST'])
def user():
    if request.method == 'GET':
        #get请求参数写在url中,127.0.0.1:5000/user/?name=fang&passwd=123
        name = request.args.get('name')
        passwd = request.args.get('passwd')
        return "this is user center.GET.name {}, passwd {}.".format(name, passwd)
    else:
        #post请求的参数通过json传给服务器
        name = request.form.get('name')
        passwd = request.form.get('passwd')
        return "this is user center.POST."

# url = 127.0.0.1:5000/user/xxx
@app.route('/user/<types>/')
def userCenter(types):
    return "this is user's " + types + " center.."




# 网站开始运行
app.run()

#if __name__ == '__main__':
#    app.run()


