"use strict";

const CLIENT_WIDTH = 350;
const CLIENT_HEIGHT = 350;

//
// 鼠标事件相关
//
function MouseState()
{
	this.leftButton = false;
	this.x = 0;
	this.y = 0;
	this.changed = false;
}

const ButtonState = {
	UP: 0,
	HELD: 1,
	RELEASED: 2,
	PRESSED: 3
};

function MouseStateTracker()
{
	const UP = 0;
	const HELD = 1;
	const RELEASED = 2;
	const PRESSED = 3;
	
	this.leftButton = UP;
	
	this.update = function(state)
	{
		if (this.leftButton === ButtonState.UP && state.leftButton === true)
			this.leftButton = ButtonState.PRESSED;
		else if (this.leftButton === ButtonState.PRESSED && state.leftButton === true)
			this.leftButton = ButtonState.HELD;
		else if (this.leftButton != ButtonState.UP && this.leftButton != ButtonState.RELEASED && state.leftButton === false)
			this.leftButton = ButtonState.RELEASED;
		else
			this.leftButton = ButtonState.UP;
	}
}

//
// 鼠标拾取射线计算
//
function screen2Ray(x, y, View, Proj)
{
	let d = vec3.fromValues(-1.0, 1.0, 0.0);
	let v = vec3.fromValues(x, y, 1.0);
	
	let scale = vec3.fromValues(2.0 / CLIENT_WIDTH, -2.0 / CLIENT_HEIGHT, 1.0);
	
	let offset = d;
	
	let InvView = mat4.create();
	let InvProj = mat4.create();
	mat4.invert(InvView, View);
	mat4.invert(InvProj, Proj);
	
	let target = vec3.create();
	vec3.multiply(target, v, scale);
	vec3.add(target, target, offset);
	vec3.transformMat4(target, target, InvProj);
	vec3.transformMat4(target, target, InvView);
	
	let from = new CANNON.Vec3(8.0, 6.0, 8.0);
	let to = new CANNON.Vec3(target[0], target[1], target[2]);
	
	let res = new CANNON.Ray(from, to); 
	
	return res;
}

function isPowerOf2(value) {
	return (value & (value - 1)) === 0;
}

function loadImageAndCreateTexture(gl, url) {
	var tex = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, tex);
	gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, 1, 1, 0, gl.RGBA, gl.UNSIGNED_BYTE,
		new Uint8Array([0, 0, 0, 255]));
	// 异步加载图片
	var img = new Image();
	img.src = url;
	img.addEventListener('load', function() {
	
		gl.bindTexture(gl.TEXTURE_2D, tex);
		gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, img);
	
		if (isPowerOf2(img.width) && isPowerOf2(img.height)) {
			gl.generateMipmap(gl.TEXTURE_2D);
		} 
		else {
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		};
	});
 
	return tex;
}


function main() {
	
	// 鼠标相关
	let clickPosX = -1, clickPosY = -1;
	let slideDelay = 0.05, currDelay = 0.0;
	let directionLocked = false;
	let lastMouseState = new MouseState();
	let mouseState = new MouseState();
	let mouseTracker = new MouseStateTracker();
	// 旋转记录
	let rotationRecord = {};
	// 游戏状态
	const GAME_PREPARING = 0;
	const GAME_READY = 1;
	const GAME_PLAYING = 2;
	const GAME_FINISHED = 3;
	let gameStatus = GAME_PREPARING;
	let shuffleSeq = [];
	let shuffleStep = 0;
	// 游戏用时
	let gameTime = 0.0;
	// 观察角度记录
	let cameraTheta = Math.PI / 4;
	
	
	// 获取画板
	let canvas = document.getElementById('rubik');

	// 创建webgl上下文
	let gl = canvas.getContext("webgl", {antialias: true});
	if (!gl) {
		console.log("您的浏览器不支持webgl！");
	}
	
	// 开启深度测试、背面剔除
	gl.enable(gl.CULL_FACE);
    gl.enable(gl.DEPTH_TEST);
	//获取GLSL文本
	let vShaderSource = document.getElementById('vertex-shader').text;
	let fShaderSource = document.getElementById('fragment-shader').text;
	//创建、编译和链接着色器
	let program = initShader(gl, vShaderSource, fShaderSource);

	// 绑定着色器中的所有attribute变量和uniform常量
	program.ALoc = {};
	program.ALoc.pos = gl.getAttribLocation(program, 'a_pos');
	program.ALoc.tex = gl.getAttribLocation(program, 'a_tex');
	program.ULoc = {};
	program.ULoc.World = gl.getUniformLocation(program, 'u_World');
	program.ULoc.View = gl.getUniformLocation(program, 'u_View');
	program.ULoc.Proj = gl.getUniformLocation(program, 'u_Proj');

	// 创建采样器
	program.ULoc.Sampler = gl.getUniformLocation(program, 'u_image');
	
	// 物理世界与碰撞检测相关
	program.worldCube = new CANNON.World();
	program.worldCube.gravity.set(0, 0, 0);
	program.worldCube.broadphase = new CANNON.NaiveBroadphase();
	program.worldPlane = new CANNON.World();
	program.worldCube.gravity.set(0, 0, 0);
	program.worldCube.broadphase = new CANNON.NaiveBroadphase();
	
	
	//创建魔方
	let rubik = new Rubik(gl, program);
	rubik.resetCube();
	
	//使画布的像素数和显示大小匹配
	webglUtils.resizeCanvasToDisplaySize(gl.canvas);

	//设置视口
	gl.viewport(0, 0, gl.canvas.width, gl.canvas.height);
	
	//应用着色器
	gl.useProgram(program);
	
	var then = 0;
	
	let View = mat4.create();
	let Proj = mat4.create();
	
	//
	// 鼠标事件
	//
	canvas.addEventListener('mousedown', function(e) {
		lastMouseState.x = mouseState.x;
		lastMouseState.y = mouseState.y;
		mouseState.x = (e.clientX - canvas.getBoundingClientRect().left);
		mouseState.y = (e.clientY - canvas.getBoundingClientRect().top);
		mouseState.leftButton = true;
		mouseState.changed = true;
	});
	
	canvas.addEventListener('mousemove', function(e) {
		lastMouseState.x = mouseState.x;
		lastMouseState.y = mouseState.y;
		mouseState.x = (e.clientX - canvas.getBoundingClientRect().left);
		mouseState.y = (e.clientY - canvas.getBoundingClientRect().top);
		mouseState.leftButton = (e.buttons > 0);
		mouseState.changed = true;
	});
	
	canvas.addEventListener('mouseup', function(e) {
		lastMouseState.x = mouseState.x;
		lastMouseState.y = mouseState.y;
		mouseState.x = (e.clientX - canvas.getBoundingClientRect().left);
		mouseState.y = (e.clientY - canvas.getBoundingClientRect().top);
		mouseState.leftButton = false;
		mouseState.changed = true;
	});
	
	//
	// 触屏事件
	//
	canvas.addEventListener('touchstart', function(e) {
		if (event.targetTouches.length == 1) {
			event.preventDefault();
			lastMouseState.x = mouseState.x;
			lastMouseState.y = mouseState.y;
			mouseState.x = Math.trunc(e.targetTouches[0].clientX - canvas.getBoundingClientRect().left);
			mouseState.y = Math.trunc(e.targetTouches[0].clientY - canvas.getBoundingClientRect().top);
			mouseState.leftButton = true;
			mouseState.changed = true;
		}
		
	});
	
	canvas.addEventListener('touchmove', function(e) {
		if (event.targetTouches.length == 1) {
			event.preventDefault();
			lastMouseState.x = mouseState.x;
			lastMouseState.y = mouseState.y;
			mouseState.x = Math.trunc(e.targetTouches[0].clientX - canvas.getBoundingClientRect().left);
			mouseState.y = Math.trunc(e.targetTouches[0].clientY - canvas.getBoundingClientRect().top);
			mouseState.changed = true;
		}
	});
	
	canvas.addEventListener('touchend', function(e) {
		if (event.targetTouches.length == 0) {
			event.preventDefault();
			lastMouseState.x = mouseState.x;
			lastMouseState.y = mouseState.y;
			mouseState.leftButton = false;
			mouseState.changed = true;
		}
	});
	
	// 按钮事件
	let btnFinish = document.getElementById("finish");
	btnFinish.addEventListener('click', function(e) {
		rubik.resetCube();
		gameStatus = GAME_FINISHED;
		gameTime = 0.0;
	});
	
	let btnReset = document.getElementById("reset");
	btnReset.addEventListener('click', function(e) {
		rubik.resetCube();
		shuffle();
		gameStatus = GAME_PREPARING;
		document.getElementById("finish").disabled = true;
		document.getElementById("reset").disabled = true;
		gameTime = 0.0;
	});
	
	// 一开始禁止按钮
	document.getElementById('finish').disabled = true;
	document.getElementById('reset').disabled = true;
	
	// 鼠标输入
	function mouseInput(dt)
	{
		let dx = 0;
		let dy = 0;
		
		if (mouseState.changed)
		{
			dx = mouseState.x - lastMouseState.x;
			dy = mouseState.y - lastMouseState.y;
			mouseTracker.update(mouseState);
			mouseState.changed = false;
		}
			
		// 鼠标左键是否点击
		if (mouseState.leftButton)
		{
			// 此时未确定旋转方向
			if (!directionLocked)
			{
				// 此时未记录点击位置
				if (clickPosX == -1 && clickPosY == -1)
				{
					// 初次点击
					if (mouseTracker.leftButton == ButtonState.PRESSED)
					{
						// 记录点击位置
						clickPosX = mouseState.x;
						clickPosY = mouseState.y;
					}
				}
				
				// 仅当记录了点击位置才进行更新
				if (clickPosX != -1 && clickPosY != -1)
					currDelay += dt;
				// 未到达滑动延迟时间则结束
				if (currDelay < slideDelay)
					return;

				// 未产生运动则不上锁
				if (Math.abs(dx) == Math.abs(dy))
					return;

				// 开始上方向锁
				directionLocked = true;
				// 更新累积的位移变化量
				dx = mouseState.x - clickPosX;
				dy = mouseState.y - clickPosY;

				// 判断当前主要是垂直操作还是水平操作
				let isVertical = Math.abs(dx) < Math.abs(dy);

				// 找到当前鼠标点击的面索引
				let ray = screen2Ray(mouseState.x, mouseState.y, View, Proj);
				let faceIndex = rubik.hitFace(ray, program);
				
				document.getElementById("picking").innerHTML = "选取面: ";
				switch(faceIndex)
				{
					case 0: document.getElementById("picking").innerHTML += "右侧";break;
					case 1: document.getElementById("picking").innerHTML += "上侧";break;
					case 2: document.getElementById("picking").innerHTML += "左侧";break;
					default: document.getElementById("picking").innerHTML += "无";break;
				}
				
				// 若找到面，则继续查找是哪个立方体
				if (faceIndex >= 0)
				{
					let ray = screen2Ray(clickPosX, clickPosY, View, Proj);
					let cubeIndex = rubik.hitCube(ray, program);
					
					document.getElementById("picking").innerHTML += "  选取方块: " + cubeIndex;		
					
					// 当前鼠标操纵的是+X面，根据操作类型决定旋转轴
					if (faceIndex === 0)
					{
						rotationRecord.pos = isVertical ? cubeIndex[2] : cubeIndex[1];
						rotationRecord.axis = isVertical ? 2 : 1;	// 2是Z轴 1是Y轴
					}
					// 当前鼠标操纵的是+Y面，要判断平移变化量dx和dy的符号来决定旋转方向
					else if (faceIndex === 1)
					{
						// 判断异号
						let diffSign = (dx >= 0 && dy < 0 || dx < 0 && dy >= 0);
						rotationRecord.pos = diffSign ? cubeIndex[0] : cubeIndex[2];
						rotationRecord.axis = diffSign ? 0 : 2;		// 0是X轴 2是Z轴
					}
					// 当前鼠标操纵的是+Z面，根据操作类型决定旋转轴
					else if (faceIndex === 2)
					{
						rotationRecord.pos = isVertical ? cubeIndex[0] : cubeIndex[1];
						rotationRecord.axis = isVertical ? 0 : 1;	// 0是X轴 1是Y轴
					}
				}
				// 当前鼠标操纵的是空白地区，则对整个魔方旋转
				else
				{
					rotationRecord.pos = 3;
					// 水平操作是Y轴旋转
					if (!isVertical)
					{
						rotationRecord.axis = 1;
					}
					// 屏幕左半部分的垂直操作是X轴旋转
					else if (mouseState.x < 300)
					{
						rotationRecord.axis = 0;
					}
					// 屏幕右半部分的垂直操作是Z轴旋转
					else
					{
						rotationRecord.axis = 2;
					}
				}
			}

			// 上了方向锁才能进行旋转
			if (directionLocked)
			{
				// 进行旋转
				switch (rotationRecord.axis)
				{
				case 0: rubik.rotationX(rotationRecord.pos, (dy - dx) * 0.012, true);
					rotationRecord.dTheta += (dx - dy) * 0.012;
					break;
				case 1: rubik.rotationY(rotationRecord.pos, dx * 0.012, true);
					rotationRecord.dTheta += (-dx * 0.012);
					break;
				case 2: rubik.rotationZ(rotationRecord.pos, (-dx - dy) * 0.012, true);
					rotationRecord.dTheta += (-dx - dy) * 0.012;
					break;
				}
			}
		}
		// 鼠标释放
		else if (clickPosX != -1 && clickPosY != -1)
		{
			// 释放方向锁
			directionLocked = false;
			// 滑动延迟归零
			currDelay = 0.0;
			// 坐标移出屏幕
			clickPosX = clickPosY = -1;

			// 发送完成指令，进行预旋转
			switch (rotationRecord.axis)
			{
			case 0: rubik.rotationX(rotationRecord.pos, 0.0); break;
			case 1: rubik.rotationY(rotationRecord.pos, 0.0); break;
			case 2: rubik.rotationZ(rotationRecord.pos, 0.0); break;
			}

			// 旋转值归零
			rotationRecord.dTheta = 0.0;
		}
		
		
	}
	
	//
	// 键盘事件
	//
	document.addEventListener('keydown', function(e) {
		if (rubik.isLocked)
			return;
		
		
		if (e.keyCode == 32)
		{
			// Shift + Space 重置游戏
			if (e.shiftKey)
			{
				rubik.resetCube();
				shuffle();
				gameStatus = GAME_PREPARING;
				document.getElementById("一键还原")
				gameTime = 0.0;
				return;
			}
			// Space 一键还原
			else
			{
				rubik.resetCube();
				gameStatus = GAME_FINISHED;
				gameTime = 0.0;
				return;
			}
			
		}
		
		//
		// 整个魔方旋转
		//
		
		// 公式x
		if (e.keyCode == 38)
		{
			rubik.rotationX(3, -Math.PI / 2);
			return;
		}
		// 公式x'
		if (e.keyCode == 40)
		{
			rubik.rotationX(3, Math.PI / 2);
			return;
		}
		// 公式y
		if (e.keyCode == 37)
		{
			rubik.rotationY(3, -Math.PI / 2);
			return;
		}
		// 公式y'
		if (e.keyCode == 39)
		{
			rubik.rotationY(3, Math.PI / 2);
			return;
		}
		// 公式z
		if (e.keyCode == 78)
		{
			rubik.rotationZ(3, Math.PI / 2);
			return;
		}
		// 公式z'
		if (e.keyCode == 77)
		{
			rubik.rotationZ(3, -Math.PI / 2);
			return;
		}
		
		//
		// 两层魔方旋转
		//
		
		if (e.shiftKey === true)
		{
			// 公式r
			if (e.keyCode == 73)
			{
				rubik.rotationX(-2, -Math.PI / 2);
				return;
			}
			// 公式r'
			if (e.keyCode == 75)
			{
				rubik.rotationX(-2, Math.PI / 2);
				return;
			}
			// 公式u
			if (e.keyCode == 74)
			{
				rubik.rotationY(-2, -Math.PI / 2);
				return;
			}
			// 公式u'
			if (e.keyCode == 76)
			{
				rubik.rotationY(-2, Math.PI / 2);
				return;
			}
			// 公式f
			if (e.keyCode == 85)
			{
				rubik.rotationZ(-2, Math.PI / 2);
				return;
			}
			// 公式f'
			if (e.keyCode == 79)
			{
				rubik.rotationZ(-2, -Math.PI / 2);
				return;
			}
			
			// 公式l'
			if (e.keyCode == 87)
			{
				rubik.rotationX(-1, -Math.PI / 2);
				return;
			}
			// 公式l
			if (e.keyCode == 83)
			{
				rubik.rotationX(-1, Math.PI / 2);
				return;
			}
			// 公式d'
			if (e.keyCode == 65)
			{
				rubik.rotationY(-1, -Math.PI / 2);
				return;
			}
			// 公式d
			if (e.keyCode == 68)
			{
				rubik.rotationY(-1, Math.PI / 2);
				return;
			}
			// 公式b'
			if (e.keyCode == 81)
			{
				rubik.rotationZ(-1, Math.PI / 2);
				return;
			}
			// 公式b
			if (e.keyCode == 69)
			{
				rubik.rotationZ(-1, -Math.PI / 2);
				return;
			}
		}
		
		//
		// 单层旋转
		//
		
		// 公式R
		if (e.keyCode == 73)
		{
			rubik.rotationX(2, -Math.PI / 2);
			return;
		}
		// 公式R'
		if (e.keyCode == 75)
		{
			rubik.rotationX(2, Math.PI / 2);
			return;
		}
		// 公式U
		if (e.keyCode == 74)
		{
			rubik.rotationY(2, -Math.PI / 2);
			return;
		}
		// 公式U'
		if (e.keyCode == 76)
		{
			rubik.rotationY(2, Math.PI / 2);
			return;
		}
		// 公式F
		if (e.keyCode == 85)
		{
			rubik.rotationZ(2, Math.PI / 2);
			return;
		}
		// 公式F'
		if (e.keyCode == 79)
		{
			rubik.rotationZ(2, -Math.PI / 2);
			return;
		}
		
		// 公式L'
		if (e.keyCode == 87)
		{
			rubik.rotationX(0, -Math.PI / 2);
			return;
		}
		// 公式L
		if (e.keyCode == 83)
		{
			rubik.rotationX(0, Math.PI / 2);
			return;
		}
		// 公式D'
		if (e.keyCode == 65)
		{
			rubik.rotationY(0, -Math.PI / 2);
			return;
		}
		// 公式D
		if (e.keyCode == 68)
		{
			rubik.rotationY(0, Math.PI / 2);
			return;
		}
		// 公式B'
		if (e.keyCode == 81)
		{
			rubik.rotationZ(0, Math.PI / 2);
			return;
		}
		// 公式B
		if (e.keyCode == 69)
		{
			rubik.rotationZ(0, -Math.PI / 2);
			return;
		}
		
		// 公式M
		if (e.keyCode == 84)
		{
			rubik.rotationX(1, -Math.PI / 2);
			return;
		}
		// 公式M
		if (e.keyCode == 71)
		{
			rubik.rotationX(1, Math.PI / 2);
			return;
		}
		// 公式E
		if (e.keyCode == 70)
		{
			rubik.rotationY(1, -Math.PI / 2);
			return;
		}
		// 公式E'
		if (e.keyCode == 72)
		{
			rubik.rotationY(1, Math.PI / 2);
			return;
		}
		// 公式S'
		if (e.keyCode == 82)
		{
			rubik.rotationZ(1, Math.PI / 2);
			return;
		}
		// 公式S
		if (e.keyCode == 89)
		{
			rubik.rotationZ(1, -Math.PI / 2);
			return;
		}
		
	});
	
	
	// 打乱魔方
	shuffle();
	
	requestAnimationFrame(render);
	
	function render(now) 
	{
	
		// ******************
		// 更新部分
		// ******************	

		// 信息
		document.getElementById("gametime").innerHTML="用时: " + gameTime.toFixed(3) + "s";
		document.getElementById("xycoordinates").innerHTML="鼠标位置: (" + mouseState.x +
			"," + mouseState.y + ")";
		document.getElementById("helper").innerHTML="空格: 一键还原，Shift+空格: 重置";
		document.getElementById("helper2").innerHTML="键盘第一/二排字母、方向键和M,N操作魔方，Shift+键盘第一/二排字母实现复杂操作";
		document.getElementById("author").innerHTML="Made By MKXJun(X_Jun)";
		
		// 将毫秒变为秒
		now *= 0.001;
		
		// 计算帧间隔
		var deltaTime = now - then;
		
		then = now;
	
		if (gameStatus == GAME_PREPARING)
		{
			if (shuffleStep === 30 && rubik.isLocked === false)
			{
				gameStatus = GAME_READY;
				document.getElementById('finish').disabled = false;
				document.getElementById('reset').disabled = false;
			}
			else if (rubik.isLocked === false)
			{
				switch(shuffleSeq[shuffleStep].axis)
				{
					case 0: rubik.rotationX(shuffleSeq[shuffleStep].pos, shuffleSeq[shuffleStep].dTheta);break;
					case 1: rubik.rotationY(shuffleSeq[shuffleStep].pos, shuffleSeq[shuffleStep].dTheta);break;
					case 2: rubik.rotationZ(shuffleSeq[shuffleStep].pos, shuffleSeq[shuffleStep].dTheta);break;
				}
				++shuffleStep;
			}
		}
		else if (gameStatus === GAME_READY)
		{		
			// 键盘通过事件反映
			mouseInput(deltaTime);
			
			if (rubik.isLocked === true)
				gameStatus = GAME_PLAYING;
		}
		else if (gameStatus === GAME_PLAYING)
		{
			// 键盘通过事件反映
			mouseInput(deltaTime);
			
			gameTime += deltaTime;
			
			if (rubik.isCompleted() === true)
			{
				alert("恭喜你! 最终用时:" + gameTime.toFixed(3) + "s");
				gameStatus = GAME_FINISHED;
			}
		}
		else if (gameStatus === GAME_FINISHED)
		{
			// 键盘通过事件反映
			mouseInput(deltaTime);
		}
		
		// 更新魔方
		rubik.update(deltaTime);
		
		// ******************
		// 渲染部分
		// ******************	

		//清除canvas
		gl.clearColor(0.458823, 0.427451, 0.513725, 1);
		gl.clear(gl.COLOR_BUFFER_BIT);
		
		gl.bindBuffer(gl.ARRAY_BUFFER, rubik.posTexBuffer);
		
		gl.enableVertexAttribArray(program.ALoc.pos);
		gl.vertexAttribPointer(program.ALoc.pos, 3, gl.FLOAT, false, 20, 0);
			
		gl.enableVertexAttribArray(program.ALoc.tex);
		gl.vertexAttribPointer(program.ALoc.tex, 2, gl.FLOAT, false, 20, 12);
			
		gl.uniform1i(program.ULoc.Sampler, 0);
		
		
		mat4.lookAt(View, vec3.fromValues(8.0, 6.0, 8.0),
			vec3.fromValues(-0.707, -0.5, -0.707),
			vec3.fromValues(0.0, 1.0, 0.0));
		mat4.perspective(Proj, glMatrix.toRadian(50), 1.0, 0.1, 1000.0);
		
		
		gl.uniformMatrix4fv(program.ULoc.View, false, View);
		gl.uniformMatrix4fv(program.ULoc.Proj, false, Proj);
			
		rubik.draw(gl, program);
		
		requestAnimationFrame(render);
	}

	function shuffle()
	{
		shuffleStep = 0;
		for (var i = 0; i < 30; ++i)
		{
			shuffleSeq[i] = {
				axis: Math.trunc(Math.random() * 3) % 3,
				pos: Math.trunc(Math.random() * 4) % 4,
				dTheta: Math.PI / 2 * (Math.random() > 0.5 ? 1 : -1)
			};
		}
	}
}




main();