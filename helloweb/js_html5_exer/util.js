if (!window.requestAnimationFrame) {
	window.requestAnimationFrame = (window.webkitRequestAnimationFrame ||
					window.mozRequestAnimationFrame ||
					window.msRequestAnimationFrame ||
					window.oRequestAnimationFrame ||
					function (callback) {
						return window.setTimeout(callback, 17 /*~ 1000/60*/);
					});
}

if (!window.cancelAnimationFrame) {
	window.cancelAnimationFrame = (	window.cancelRequestAnimationFrame ||
					window.webkitCancelAnimationFrame || window.webkitCancelRequestAnimationFrame ||
					window.mozCancelAnimationFrame || window.mozCancelRequestAnimationFrame ||
					window.msCancelAnimationFrame || window.msCancelRequestAnimationFrame ||
					window.oCancelAnimationFrame || window.oCancelRequestAnimationFrame ||
					window.clearTimeout);
}

//1. 自己定义的全局量,可以理解为一个(类 or 键值对集合)
//2. 注意多个键值对之间以","分开, 最后的","可有可无
//3. 也可以直接 window.utils = {}; 自动为 系统预定义的全局量 window 添加一个键值对 k=utils,v={},好处是定义的全局量更少,命名空间污染小
//   (注意这种方式没有 var 新变量定义)
var utils = {
	name: "my utils",
	selfDesc: function(){
		console.log("this is a util, code ref to https://github.com/lamberta/html5-animation");
	},
};

//这将为 utils 自动添加 (captureMouse,function) 的键值对
utils.captureMouse = function (element){
	var	mouse = {x: 0, y: 0, event: null},
		body_scrollLeft = document.body.scrollLeft,
		element_scrollLeft = document.documentElement.scrollLeft,
		body_scrollTop = document.body.scrollTop,
		element_scrollTop = document.documentElement.scrollTop,
		offsetLeft = element.offsetLeft,
		offsetTop = element.offsetTop;

	//目前可以确定的是 addEventListener 针对 相同事件 的监听并不会发生 处理函数 的覆盖,而是组织成list依次调用
	element.addEventListener(	'mousemove',
					function (event) {	//这个匿名 function 是在外面的 function 内部定义,外部的 形参&local 此函数均可access
						var x, y;

						if (event.pageX || event.pageY) {
							x = event.pageX;
							y = event.pageY;
						} else {
							x = event.clientX + body_scrollLeft + element_scrollLeft;
							y = event.clientY + body_scrollTop + element_scrollTop;
						}
						x -= offsetLeft;
						y -= offsetTop;

						mouse.x = x;
						mouse.y = y;
						mouse.event = event;
					},
					false);
	return mouse;
}

//这是为移动设备使用的
utils.captureTouch = function (element){
	var	touch = {x: null, y: null, isPressed: false, event: null},
		body_scrollLeft = document.body.scrollLeft,
		element_scrollLeft = document.documentElement.scrollLeft,
		body_scrollTop = document.body.scrollTop,
		element_scrollTop = document.documentElement.scrollTop,
		offsetLeft = element.offsetLeft,
		offsetTop = element.offsetTop;

	element.addEventListener(	'touchstart', 
					function (event) {
						touch.isPressed = true;
						touch.event = event;
					}, 
					false);

	element.addEventListener(	'touchend',
					function (event) {
						touch.isPressed = false;
						touch.x = null;
						touch.y = null;
						touch.event = event;
					},
					false);

	element.addEventListener(	'touchmove',
					function (event) {
						var x, y,
						touch_event = event.touches[0]; //first touch

						if (touch_event.pageX || touch_event.pageY) {
							x = touch_event.pageX;
							y = touch_event.pageY;
						} else {
							x = touch_event.clientX + body_scrollLeft + element_scrollLeft;
							y = touch_event.clientY + body_scrollTop + element_scrollTop;
						}
						x -= offsetLeft;
						y -= offsetTop;

						touch.x = x;
						touch.y = y;
						touch.event = event;
					},
					false);
	return touch;
}
