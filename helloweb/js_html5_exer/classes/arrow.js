/*当想以构造函数的那种方式使用键值对集合(或class)的时候,使用下面这种构造
function Util (name){		//约定作为构造函数的函数以大写打头
	this.name = name;		//自动创建了存储键值对的集合实体 this
	this.selfDesc = function(){	//通过this access的key被认为是类似public成员,没有通过this access的那都是类似private的了
		console.log("this is "+ this.name +", code ref to https://github.com/lamberta/html5-animation");
	};
	//函数会将 this 作为返回值
}
var objA = new Util("fangUtil");
objA.selfDesc();

与静态存在的键值对集合不同,这种动态创建的键值对集合,后面再想添加成员的时候,可以这样做:
Util.prototype.func = function(){	//自动添加了新的键值对,且被所有通过构造创建的键值对集合动态继承
}
*/

function Arrow(){
	this.x = 0;
	this.x = 0;
	this.color = "#ffff00";
	this.rotation = 0;	//需要旋转的弧度
}

Arrow.prototype.draw = function (context){	//调用 canvas 的绘图api 绘制一个arrow
	context.save();		//保存上下文信息
	context.translate(this.x,this.y);	//这个应该是设定坐标原点的位置
	context.rotate(this.rotation);		//这个是旋转坐标系?
	context.lineWidth = 2;
	context.fillStyle = this.color;

	context.beginPath();	//开始画线条
	context.moveTo(-50,-25);
	context.lineTo(0,-25);
	context.lineTo(0,-50);
	context.lineTo(50,0);
	context.lineTo(0,50);
	context.lineTo(0,25);
	context.lineTo(-50,25);
	context.lineTo(-50,-25);
	context.closePath();	//结束画线
	context.fill();		//填充封闭区域
	context.stroke();	//这个开起来是一个类似render的操作
	context.restore();	//恢复上下文信息
}
