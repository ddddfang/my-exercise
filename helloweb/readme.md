<js编程全解>的阅读笔记

* === 与 ==
    * ==会自动进行数据类型的转换
    * ===不会转换数据类型

* 变量不具有类型，变量存在只是为了 持有某个值 或 指代不具有名字的对象
* **基本数据类型**包括（`数值` `字符串` `布尔型` `null` `undefine`），将其赋值给变量采用 **值** 传递方式（即js会自动malloc一个tmp，并把右值赋值过来）. 其中**字符串**作为基本数据类型之一，理解为值传递，但内部采用引用方式实现（const指针即代表这个string），这是出于效率考虑. 
* **对象**，将其赋值给变量采用 引用 传递方式，会将对象的引用赋值给变量（所以对象改变的话会影响到所有引用该对象的变量）. 
* **函数function**理解为特殊的对象. 
* 数组 日期 正则表达式
* ES6可以使用let替代var声明一个变量,var允许任意位置声明变量,甚至重写已经声明的变量(看起来像是声明一个已经声明的变量) let不允许两次声明同名的变量,行为和作用于都合c/java更贴近


* 全局变量（以及全局函数名）是全局对象的属性，全局对象通过全局this指针拿到。客户端js中的window对象，就是一个全局this的引用(要知道this还有局部this呢)
* 表达式 `(全局变量 in 全局this)` 将返回true （利用这个可以判断一个变量是否被声明过）

* 面向对象的 3 要素，即封装、继承与多态（OOP）
* 在 JavaScript 中，一切都是**对象**。对象之间的协作（消息收发）通过属性访问（以及方法的调用）来实现。而对象之间的共性，则是通过继承同一个对象的性质的方式来实现。JavaScript 通过基于原型的形式来实现继承
* js通常会作如下约定：若只需一个对象实例，则不会去设计一个类，而是会使用 对象字面量（单例模式）
* js中函数参数较多，可以通过将参数揉成一个 对象字面量 传入，这样还不会出现参数顺序写错的问题
* js中函数若是想返回多个值，也是通过将他们揉成一个 对象字面量 return的（p101），（return [3,4,5] 是js1.7增强功能，返回一个array）
* js可以通过对象字面量来替代构造函数，使用返回对象字面量的函数，与通过 new 表达式来调用构造函数，是两种不同风格的生成对象的手段

``` js
//手段1
function createObject() {
	return {	x:3, y:2, z:2,
			getDistance:function() {
				return Math.sqrt(this.x * this.x + this.y * this.y + this.z * this.z);
			}};
}
var obj = createObject();
print(obj.getDistance());

//手段2 构造函数（类的定义）p102
function MyClass(x, y) {	//一般将作为构造函数调用而存在的函数首字母大写
	this.x = x;
	this.y = y;
	this.show = function() {
		print(this.x, this.y);
	}
}
//由 new 表达式调用的函数就是构造函数,任何函数都可以通过 new 表达式调用,构造函数在最后会隐式地执行 return this（建议不要显式写）
var obj = new MyClass(3, 2);
```
俩问题：
    1. 由于所有的实例都是复制了同一个方法所定义的实体，所以效率（内存效率与执行效率）低下。（原型继承来解决）
    2. 无法对属性值进行访问控制（private 或 public 等）。（闭包来解决）

* js实现默认参数的习惯用法（js本身不支持默认参数）

``` js
function getDistance(pos) {	//通过 || 运算可以将参数作为布尔型来判断真假，若调用函数时没有实参参数的值则为undefined
	pos = pos || { x:0, y:0, z:0 }; // 如果没有收到参数 pos 的话，则使用默认值
	return Math.sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
}
```



* 模板字面量 
``` js
var book = {
    name = "learn js",
};

console.log("you are reading" + book.name +"\n");
console.log(`you are reading ${book.name}`);    //包含在``中的就是一个模板字面量,要插入的变量包含在${}中即可,换行就是换行(当然这里没有加)
```

* ES6新增的函数特性:
```js
var circleArea = function circleArea(r) {//传统的函数定义方式
    var PI = 3.14;
    var area = PI * r * r;
return area;
};
console.log(circleArea(2));


let circleArea = (r) => { //circleArea 就是一个匿名函数(使用 => 也暗含函数就是一个映射),若函数只有一句,还可以更简单, let circleArea = (r) => 3.14*r*r;
    const PI = 3.14;
    let area = PI * r * r;
    return area;
}
console.log(circleArea(2));



function sum (x = 1,y = 2, z = 3) {//ES6新增的参数默认值
    return x + y + z;
};

/×
ES6之前上述代码只可以这样写:
×/


```



