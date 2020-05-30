"use strict"

function Cube()
{
	this.faceColors = [1, 2, 3, 4, 5, 6];	// 六个面的颜色，索引0-5分别对应+X, -X, +Y, -Y, +Z, -Z面
											// 0黑色, 1白色, 2黄色, 3绿色, 4蓝色, 5红色, 6橙色
	this.pos = [0.0, 0.0, 0.0];				// 旋转结束后中心所处位置
	this.rotation = [0.0, 0.0, 0.0];		// 仅允许存在单轴旋转，记录当前分别绕x轴, y轴, z轴旋转的弧度
	this.getWorldMatrix = function() {
		let RX = mat4.create();
		let RY = mat4.create();
		let RZ = mat4.create();
		
		mat4.fromXRotation(RX, this.rotation[0]);
		mat4.fromYRotation(RY, this.rotation[1]);
		mat4.fromZRotation(RZ, this.rotation[2]);
		
		let finalPos = vec3.create();
		vec3.transformMat4(finalPos, this.pos, RZ);
		vec3.transformMat4(finalPos, finalPos, RX);
		vec3.transformMat4(finalPos, finalPos, RY);
	
		let T = mat4.create();
		mat4.fromTranslation(T, finalPos);
		
		let World = mat4.create();
		mat4.mul(World, RX, RZ);
		mat4.mul(World, RY, World);
		mat4.mul(World, T, World);
		
		return World;
	}
}

function Rubik(gl, program)
{
	// 当前是否正在拖动
	this.isPressed = false;
	// 当前是否有动画在播放
	this.isLocked = false;
	// 当前自动旋转的速度
	this.rotationSpeed = Math.PI * 3;
	
	const RubikFace_PosX = 0;	// +X面
	const RubikFace_NegX = 1;	// -X面
	const RubikFace_PosY = 2;	// +Y面
	const RubikFace_NegY = 3;	// -Y面
	const RubikFace_PosZ = 4;	// +Z面
	const RubikFace_NegZ = 5;	// -Z面
	
	
	
	// 27个立方体
	this.cubes = new Array();
	this.cubes[0] = new Array();
	this.cubes[1] = new Array();
	this.cubes[2] = new Array();
	
	
	
	for (var i = 0; i < 3; ++i)
	{
		this.cubes[i][0] = new Array();
		this.cubes[i][1] = new Array();
		this.cubes[i][2] = new Array();
		for (var j = 0; j < 3; ++j)
		{
			this.cubes[i][j][0] = new Cube();
			this.cubes[i][j][1] = new Cube();
			this.cubes[i][j][2] = new Cube();
			for (var k = 0; k < 3; ++k)
			{
				let boxShape = new CANNON.Box(new CANNON.Vec3(1.0, 1.0, 1.0));
				let cubeBody = new CANNON.Body({
					mass: 0,
					shape: boxShape,
					collisionResponse: false,
					position: new CANNON.Vec3(-2.0 + 2.0 * i, -2.0 + 2.0 * j, -2.0 + 2.0 * k)
				});
				program.worldCube.addBody(cubeBody);
			}
		}
	}
	
	// 添加三个薄片
	// +X面
	let boxShape = new CANNON.Box(new CANNON.Vec3(0.001, 3.0, 3.0));
	let cubeBody = new CANNON.Body({
		mass: 0,
		shape: boxShape,
		collisionResponse: false,
		position: new CANNON.Vec3(3.0, 0.0, 0.0)
	});
	program.worldPlane.addBody(cubeBody);
	// +Y面
	boxShape = new CANNON.Box(new CANNON.Vec3(3.0, 0.001, 3.0));
	cubeBody = new CANNON.Body({
		mass: 0,
		shape: boxShape,
		collisionResponse: false,
		position: new CANNON.Vec3(0.0, 3.0, 0.0)
	});
	program.worldPlane.addBody(cubeBody);
	// +Z面
	boxShape = new CANNON.Box(new CANNON.Vec3(3.0, 3.0, 0.001));
	cubeBody = new CANNON.Body({
		mass: 0,
		shape: boxShape,
		collisionResponse: false,
		position: new CANNON.Vec3(0.0, 0.0, 3.0)
	});
	program.worldPlane.addBody(cubeBody);
	
	// 位置、纹理坐标缓冲区
	this.posTexBuffer = gl.createBuffer();
	gl.bindBuffer(gl.ARRAY_BUFFER, this.posTexBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, new Float32Array([
		// pos				 tex
		// +X面
		1.0, -1.0, 1.0,		0.0, 0.0,
		1.0, -1.0, -1.0,	1.0, 0.0,
		1.0, 1.0, -1.0,		1.0, 1.0,
		1.0, 1.0, -1.0,		1.0, 1.0,
		1.0, 1.0, 1.0,		0.0, 1.0,
		1.0, -1.0, 1.0,		0.0, 0.0,
		// -X面
		-1.0, -1.0, -1.0,	0.0, 0.0,
		-1.0, -1.0, 1.0,	1.0, 0.0,
		-1.0, 1.0, 1.0,		1.0, 1.0,
		-1.0, 1.0, 1.0,		1.0, 1.0,
		-1.0, 1.0, -1.0,	0.0, 1.0,
		-1.0, -1.0, -1.0,	0.0, 0.0,
		// +Y面
		-1.0, 1.0, 1.0,		0.0, 0.0,
		1.0, 1.0, 1.0,		1.0, 0.0,	
		1.0, 1.0, -1.0,		1.0, 1.0,
		1.0, 1.0, -1.0,		1.0, 1.0,
		-1.0, 1.0, -1.0,	0.0, 1.0,
		-1.0, 1.0, 1.0,		0.0, 0.0,
		// -Y面
		-1.0, -1.0, -1.0,	0.0, 0.0,
		1.0, -1.0, -1.0,	1.0, 0.0,
		1.0, -1.0, 1.0,		1.0, 1.0,
		1.0, -1.0, 1.0,		1.0, 1.0,
		-1.0, -1.0, 1.0,	0.0, 1.0,
		-1.0, -1.0, -1.0,	0.0, 0.0,
		// +Z面
		-1.0, -1.0, 1.0,	0.0, 0.0,
		1.0, -1.0, 1.0,		1.0, 0.0,
		1.0, 1.0, 1.0,		1.0, 1.0,
		1.0, 1.0, 1.0,		1.0, 1.0,
		-1.0, 1.0, 1.0,		0.0, 1.0,
		-1.0, -1.0, 1.0,	0.0, 0.0,
		// -Z面
		1.0, -1.0, -1.0,	0.0, 0.0,
		-1.0, -1.0, -1.0,	1.0, 0.0,
		-1.0, 1.0, -1.0,	1.0, 1.0,
		-1.0, 1.0, -1.0,	1.0, 1.0,
		1.0, 1.0, -1.0,		0.0, 1.0,
		1.0, -1.0, -1.0,	0.0, 0.0,
	]), gl.STATIC_DRAW);
	
	// 所用到的纹理
	this.textures = [
		loadImageAndCreateTexture(gl, './textures/Black.png'),
		loadImageAndCreateTexture(gl, './textures/White.png'),
		loadImageAndCreateTexture(gl, './textures/Yellow.png'),
		loadImageAndCreateTexture(gl, './textures/Green.png'),
		loadImageAndCreateTexture(gl, './textures/Blue.png'),
		loadImageAndCreateTexture(gl, './textures/Red.png'),
		loadImageAndCreateTexture(gl, './textures/Orange.png'),
	];
	
	// 重置魔方
	this.resetCube = function()
	{
		for (var i = 0; i < 3; ++i)
		{
			for (var j = 0; j < 3; ++j)
			{
				this.cubes[i][j][0].faceColors = [0, 0, 0, 0, 0, 0];
				this.cubes[i][j][1].faceColors = [0, 0, 0, 0, 0, 0];
				this.cubes[i][j][2].faceColors = [0, 0, 0, 0, 0, 0];
			}
		}
		
		for (var i = 0; i < 3; ++i)
		{
			for (var j = 0; j < 3; ++j)
			{	
				
				
				this.cubes[2][i][j].faceColors[0] = 1;
				this.cubes[0][i][j].faceColors[1] = 2;
				
				this.cubes[j][2][i].faceColors[2] = 3;
				this.cubes[j][0][i].faceColors[3] = 4;
				
				this.cubes[i][j][2].faceColors[4] = 5;
				this.cubes[i][j][0].faceColors[5] = 6;
				
				for (var k = 0; k < 3; ++k)
				{
					this.cubes[i][j][k].pos = [-2.0 + 2.0 * i, -2.0 + 2.0 * j, -2.0 + 2.0 * k];
					this.cubes[i][j][k].rotation = [0.0, 0.0, 0.0];
				}
			}
		}
	}
	
	this.update = function(dt)
	{
		if (this.isLocked === true)
		{
			var finishCount = 0;
			for (var i = 0; i < 3; ++i)
			{
				for (var j = 0; j < 3; ++j)
				{
					for (var k = 0; k < 3; ++k)
					{
						// 令x，y, z轴向旋转角度逐渐归0
						// x轴
						var dTheta = (this.cubes[i][j][k].rotation[0] > 0.0 ? -1.0 : 1.0) * dt * this.rotationSpeed;
						if (Math.abs(this.cubes[i][j][k].rotation[0]) < Math.abs(dTheta))
						{
							this.cubes[i][j][k].rotation[0] = 0.0;
							finishCount++;
						}
						else
						{
							this.cubes[i][j][k].rotation[0] += dTheta;
						}
						// y轴
						var dTheta = (this.cubes[i][j][k].rotation[1] > 0.0 ? -1.0 : 1.0) * dt * this.rotationSpeed;
						if (Math.abs(this.cubes[i][j][k].rotation[1]) < Math.abs(dTheta))
						{
							this.cubes[i][j][k].rotation[1] = 0.0;
							finishCount++;
						}
						else
						{
							this.cubes[i][j][k].rotation[1] += dTheta;
						}
						// z轴
						var dTheta = (this.cubes[i][j][k].rotation[2] > 0.0 ? -1.0 : 1.0) * dt * this.rotationSpeed;
						if (Math.abs(this.cubes[i][j][k].rotation[2]) < Math.abs(dTheta))
						{
							this.cubes[i][j][k].rotation[2] = 0.0;
							finishCount++;
						}
						else
						{
							this.cubes[i][j][k].rotation[2] += dTheta;
						}
					}
				}
			}
			
			// 所有方块都结束动画才能解锁
			if (finishCount === 81)
				this.isLocked = false;
		}
	}
	
	
	// 绘制魔方
	this.draw = function(gl, program)
	{
		let World = mat4.create();
		
		for (var i = 0; i < 3; ++i)
		{
			for (var j = 0; j < 3; ++j)
			{
				for (var k = 0; k < 3; ++k)
				{
					for (var face = 0; face < 6; face++)
					{
						gl.bindTexture(gl.TEXTURE_2D, this.textures[this.cubes[i][j][k].faceColors[face]]);
						
						World = this.cubes[i][j][k].getWorldMatrix();
						gl.uniformMatrix4fv(program.ULoc.World, false, World); 
						
						gl.drawArrays(gl.TRIANGLES, face * 6, 6);
					}
				}
			}
		}
	}
	
	// 当前魔方是否还原
	this.isCompleted = function()
	{
		let posX = this.cubes[2][0][0].faceColors[0];
		let negX = this.cubes[0][0][0].faceColors[1];
		let posY = this.cubes[0][2][0].faceColors[2];
		let negY = this.cubes[0][0][0].faceColors[3];
		let posZ = this.cubes[0][0][2].faceColors[4];
		let negZ = this.cubes[0][0][0].faceColors[5];
		
		for (var j = 0; j < 3; ++j)
			for (var k = 0; k < 3; ++k)
				if (this.cubes[2][j][k].faceColors[0] != posX || this.cubes[0][j][k].faceColors[1] != negX)
					return false;
	
		for (var k = 0; k < 3; ++k)
			for (var i = 0; i < 3; ++i)
				if (this.cubes[i][2][k].faceColors[2] != posY || this.cubes[i][0][k].faceColors[3] != negY)
					return false;
				
		for (var i = 0; i < 3; ++i)
			for (var j = 0; j < 3; ++j)
				if (this.cubes[i][j][2].faceColors[4] != posZ || this.cubes[i][j][0].faceColors[5] != negZ)
					return false;		
	
		return true;
	}
	
	// 如果拾取中立方体，返回拾取距离最近的立方体其索引和距离{[i, j, k], dist}
	// 反之则返回[-1, -1, -1]
	this.hitCube = function(ray, program)
	{
		// 初始化世界场景中的物体
		for (var i = 0; i < 3; ++i)
		{
			for (var j = 0; j < 3; ++j)
			{	
				for (var k = 0; k < 3; ++k)
				{
					let RX = mat4.create();
					let RY = mat4.create();
					let RZ = mat4.create();
					
					mat4.fromXRotation(RX, this.cubes[i][j][k].rotation[0]);
					mat4.fromYRotation(RY, this.cubes[i][j][k].rotation[1]);
					mat4.fromZRotation(RZ, this.cubes[i][j][k].rotation[2]);
					
					let finalPos = vec3.create();
					vec3.transformMat4(finalPos, this.cubes[i][j][k].pos, RZ);
					vec3.transformMat4(finalPos, finalPos, RX);
					vec3.transformMat4(finalPos, finalPos, RY);
					
					program.worldCube.bodies[i * 9 + j * 3 + k].position = new CANNON.Vec3(
						finalPos[0], finalPos[1], finalPos[2]);
					
					program.worldCube.bodies[i * 9 + j * 3 + k].quaternion = new CANNON.Quaternion();
					program.worldCube.bodies[i * 9 + j * 3 + k].quaternion.setFromEuler(
						this.cubes[i][j][k].rotation[0],
						this.cubes[i][j][k].rotation[1],
						this.cubes[i][j][k].rotation[2],
						'ZXY');
						
				}
			}
		}
		
		let raycastResult = new CANNON.RaycastResult();
		program.worldCube.raycastClosest(ray.from, ray.to, {}, raycastResult);
		if (raycastResult.hasHit === false)
			return [-1, -1, -1];
		else
		{
			for (var i = 0; i < 27; ++i)
				if (raycastResult.body === program.worldCube.bodies[i])
				{
					let res = [Math.trunc(i / 9), Math.trunc(i / 3) % 3, i % 3];
					return res;
				}
		}
	}
	
	this.hitFace = function(ray, program)
	{
		let raycastResult = new CANNON.RaycastResult();
		program.worldPlane.raycastClosest(ray.from, ray.to, {}, raycastResult);
		if (raycastResult.hasHit === false)
			return -1;
		else
		{
			for (var i = 0; i < 3; ++i)
				if (raycastResult.body === program.worldPlane.bodies[i])
					return i;		// 0为+X面, 1为+Y面, 2为+Z面
		}
	}
	
	
	
	// pos的取值为0-2时，绕X轴旋转魔方指定层 
	// pos的取值为-1时，绕X轴旋转魔方pos为0和1的两层
	// pos的取值为-2时，绕X轴旋转魔方pos为1和2的两层
	// pos的取值为3时，绕X轴旋转整个魔方
	this.rotationX = function(pos, dTheta, isPressed = false)
	{
		if (this.isLocked === false)
		{
			// 检验当前是否为键盘操作
			// 可以认为仅当键盘操作时才会产生绝对值为pi/2的倍数(不包括0)的瞬时值
			let isKeyOp =  Math.trunc(Math.round(dTheta / (Math.PI / 2))) != 0 &&
				(Math.abs(dTheta % (Math.PI / 2)) < 0.000001);
			// 键盘输入和鼠标操作互斥，拒绝键盘的操作
			if (this.isPressed === true && isKeyOp === true)
			{
				return;
			}

			this.isPressed = isPressed;

			// 更新旋转状态
			for (var j = 0; j < 3; ++j)
				for (var k = 0; k < 3; ++k)
				{
					switch (pos)
					{
					case 3: this.cubes[0][j][k].rotation[0] += dTheta;
					case -2: this.cubes[1][j][k].rotation[0] += dTheta;
						this.cubes[2][j][k].rotation[0] += dTheta;
						break;
					case -1: this.cubes[0][j][k].rotation[0] += dTheta; 
						this.cubes[1][j][k].rotation[0] += dTheta; 
						break;
					
					default: this.cubes[pos][j][k].rotation[0] += dTheta;
					}
					
				}
					

			// 鼠标或键盘操作完成
			if (this.isPressed === false)
			{
				// 开始动画演示状态
				this.isLocked = true;

				// 进行预旋转
				this.preRotationX(isKeyOp);
			}
		}
	}
	
	// pos的取值为3时，绕Y轴旋转魔方指定层 
	// pos的取值为-1时，绕Y轴旋转魔方pos为0和1的两层
	// pos的取值为-2时，绕Y轴旋转魔方pos为1和2的两层
	// pos的取值为3时，绕Y轴旋转整个魔方
	this.rotationY = function(pos, dTheta, isPressed = false)
	{
		if (this.isLocked === false)
		{
			// 检验当前是否为键盘操作
			// 可以认为仅当键盘操作时才会产生绝对值为pi/2的倍数(不包括0)的瞬时值
			let isKeyOp =  Math.trunc(Math.round(dTheta / (Math.PI / 2))) != 0 &&
				(Math.abs(dTheta % (Math.PI / 2)) < 0.000001);
			// 键盘输入和鼠标操作互斥，拒绝键盘的操作
			if (this.isPressed === true && isKeyOp === true)
			{
				return;
			}

			this.isPressed = isPressed;

			// 更新旋转状态
			for (var k = 0; k < 3; ++k)
				for (var i = 0; i < 3; ++i)
				{
					switch (pos)
					{
					case 3: this.cubes[i][0][k].rotation[1] += dTheta;
					case -2: this.cubes[i][1][k].rotation[1] += dTheta;
						this.cubes[i][2][k].rotation[1] += dTheta;
						break;
					case -1: this.cubes[i][0][k].rotation[1] += dTheta; 
						this.cubes[i][1][k].rotation[1] += dTheta; 
						break;
					
					default: this.cubes[i][pos][k].rotation[1] += dTheta;
					}
					
				}
					

			// 鼠标或键盘操作完成
			if (this.isPressed === false)
			{
				// 开始动画演示状态
				this.isLocked = true;

				// 进行预旋转
				this.preRotationY(isKeyOp);
			}
		}
	}
	
	// pos的取值为0-2时，绕Z轴旋转魔方指定层 
	// pos的取值为-1时，绕Z轴旋转魔方pos为0和1的两层
	// pos的取值为-2时，绕Z轴旋转魔方pos为1和2的两层
	// pos的取值为3时，绕Z轴旋转整个魔方
	this.rotationZ = function(pos, dTheta, isPressed = false)
	{
		if (this.isLocked === false)
		{
			// 检验当前是否为键盘操作
			// 可以认为仅当键盘操作时才会产生绝对值为pi/2的倍数(不包括0)的瞬时值
			let isKeyOp =  Math.trunc(Math.round(dTheta / (Math.PI / 2))) != 0 &&
				(Math.abs(dTheta % (Math.PI / 2)) < 0.000001);
			// 键盘输入和鼠标操作互斥，拒绝键盘的操作
			if (this.isPressed === true && isKeyOp === true)
			{
				return;
			}

			this.isPressed = isPressed;

			// 更新旋转状态
			for (var i = 0; i < 3; ++i)
				for (var j = 0; j < 3; ++j)
				{
					switch (pos)
					{
					case 3: this.cubes[i][j][0].rotation[2] += dTheta;
					case -2: this.cubes[i][j][1].rotation[2] += dTheta;
						this.cubes[i][j][2].rotation[2] += dTheta;
						break;
					case -1: this.cubes[i][j][0].rotation[2] += dTheta; 
						this.cubes[i][j][1].rotation[2] += dTheta; 
						break;
					
					default: this.cubes[i][j][pos].rotation[2] += dTheta;
					}
					
				}
					

			// 鼠标或键盘操作完成
			if (this.isPressed === false)
			{
				// 开始动画演示状态
				this.isLocked = true;

				// 进行预旋转
				this.preRotationZ(isKeyOp);
			}
		}
	}
	
	
	// 绕X轴的预旋转
	this.preRotationX = function(isKeyOp)
	{
		for (var i = 0; i < 3; ++i)
		{
			// 当前层没有旋转则直接跳过
			if (Math.abs(this.cubes[i][0][0].rotation[0]) < -0.000001)
				continue;
			// 由于此时被旋转面的所有方块旋转角度都是一样的，可以从中取一个来计算。
			// 计算归位回[-pi/4, pi/4)区间需要顺时针旋转90度的次数
			let times = Math.trunc(Math.round(this.cubes[i][0][0].rotation[0] / (Math.PI / 2)));
			// 将归位次数映射到[0, 3]，以计算最小所需顺时针旋转90度的次数
			let minTimes = (times % 4 + 4) % 4;

			// 调整所有被旋转方块的初始角度
			for (var j = 0; j < 3; ++j)
			{
				for (var k = 0; k < 3; ++k)
				{
					// 键盘按下后的变化
					if (isKeyOp)
					{
						// 顺时针旋转90度--->实际演算从-90度加到0度
						// 逆时针旋转90度--->实际演算从90度减到0度
						this.cubes[i][j][k].rotation[0] *= -1.0;
					}
					// 鼠标释放后的变化
					else
					{
						// 归位回[-pi/4, pi/4)的区间
						this.cubes[i][j][k].rotation[0] -= times * (Math.PI / 2);
					}
				}
			}
			
			let outArrs = this.getSwapIndexArray(minTimes);
			for (var idx = 0; idx < outArrs.size; ++idx)
			{
				// 对这两个立方体按规则进行面的交换
				let srcIndex = outArrs.srcIndex;
				let dstIndex = outArrs.dstIndex;
				// 若为2次顺时针旋转，则只需4次对角调换
				// 否则，需要6次邻角(棱)对换
				for (var face = 0; face < 6; ++face)
				{
					let tface = this.getTargetSwapFaceRotationX(face, minTimes);
					
					let tmp = this.cubes[i][srcIndex[idx][0]][srcIndex[idx][1]].faceColors[face];
					this.cubes[i][srcIndex[idx][0]][srcIndex[idx][1]].faceColors[face] =
						this.cubes[i][dstIndex[idx][0]][dstIndex[idx][1]].faceColors[tface];
					this.cubes[i][dstIndex[idx][0]][dstIndex[idx][1]].faceColors[tface] = tmp;
				}
			}
		}
	}
	// 绕Y轴的预旋转
	this.preRotationY = function(isKeyOp)
	{
		for (var j = 0; j < 3; ++j)
		{
			// 当前层没有旋转则直接跳过
			if (Math.abs(this.cubes[0][j][0].rotation[1]) < -0.000001)
				continue;
			// 由于此时被旋转面的所有方块旋转角度都是一样的，可以从中取一个来计算。
			// 计算归位回[-pi/4, pi/4)区间需要顺时针旋转90度的次数
			let times = Math.trunc(Math.round(this.cubes[0][j][0].rotation[1] / (Math.PI / 2)));
			// 将归位次数映射到[0, 3]，以计算最小所需顺时针旋转90度的次数
			let minTimes = (times % 4 + 4) % 4;

			// 调整所有被旋转方块的初始角度
			for (var k = 0; k < 3; ++k)
			{
				for (var i = 0; i < 3; ++i)
				{
					// 键盘按下后的变化
					if (isKeyOp)
					{
						// 顺时针旋转90度--->实际演算从-90度加到0度
						// 逆时针旋转90度--->实际演算从90度减到0度
						this.cubes[i][j][k].rotation[1] *= -1.0;
					}
					// 鼠标释放后的变化
					else
					{
						// 归位回[-pi/4, pi/4)的区间
						this.cubes[i][j][k].rotation[1] -= times * (Math.PI / 2);
					}
				}
			}
			
			let outArrs = this.getSwapIndexArray(minTimes);
			for (var idx = 0; idx < outArrs.size; ++idx)
			{
				// 对这两个立方体按规则进行面的交换
				let srcIndex = outArrs.srcIndex;
				let dstIndex = outArrs.dstIndex;
				// 若为2次顺时针旋转，则只需4次对角调换
				// 否则，需要6次邻角(棱)对换
				for (var face = 0; face < 6; ++face)
				{
					let tface = this.getTargetSwapFaceRotationY(face, minTimes);
					
					let tmp = this.cubes[srcIndex[idx][1]][j][srcIndex[idx][0]].faceColors[face];
					this.cubes[srcIndex[idx][1]][j][srcIndex[idx][0]].faceColors[face] =
						this.cubes[dstIndex[idx][1]][j][dstIndex[idx][0]].faceColors[tface];
					this.cubes[dstIndex[idx][1]][j][dstIndex[idx][0]].faceColors[tface] = tmp;
				}
			}
		}
	}
	// 绕Z轴的预旋转
	this.preRotationZ = function(isKeyOp)
	{
		for (var k = 0; k < 3; ++k)
		{
			// 当前层没有旋转则直接跳过
			if (Math.abs(this.cubes[0][0][k].rotation[2]) < -0.000001)
				continue;
			// 由于此时被旋转面的所有方块旋转角度都是一样的，可以从中取一个来计算。
			// 计算归位回[-pi/4, pi/4)区间需要顺时针旋转90度的次数
			let times = Math.trunc(Math.round(this.cubes[0][0][k].rotation[2] / (Math.PI / 2)));
			// 将归位次数映射到[0, 3]，以计算最小所需顺时针旋转90度的次数
			let minTimes = (times % 4 + 4) % 4;

			// 调整所有被旋转方块的初始角度
			for (var i = 0; i < 3; ++i)
			{
				for (var j = 0; j < 3; ++j)
				{
					// 键盘按下后的变化
					if (isKeyOp)
					{
						// 顺时针旋转90度--->实际演算从-90度加到0度
						// 逆时针旋转90度--->实际演算从90度减到0度
						this.cubes[i][j][k].rotation[2] *= -1.0;
					}
					// 鼠标释放后的变化
					else
					{
						// 归位回[-pi/4, pi/4)的区间
						this.cubes[i][j][k].rotation[2] -= times * (Math.PI / 2);
					}
				}
			}
			
			let outArrs = this.getSwapIndexArray(minTimes);
			for (var idx = 0; idx < outArrs.size; ++idx)
			{
				// 对这两个立方体按规则进行面的交换
				let srcIndex = outArrs.srcIndex;
				let dstIndex = outArrs.dstIndex;
				// 若为2次顺时针旋转，则只需4次对角调换
				// 否则，需要6次邻角(棱)对换
				for (var face = 0; face < 6; ++face)
				{
					let tface = this.getTargetSwapFaceRotationZ(face, minTimes);
					
					let tmp = this.cubes[srcIndex[idx][0]][srcIndex[idx][1]][k].faceColors[face];
					this.cubes[srcIndex[idx][0]][srcIndex[idx][1]][k].faceColors[face] =
						this.cubes[dstIndex[idx][0]][dstIndex[idx][1]][k].faceColors[tface];
					this.cubes[dstIndex[idx][0]][dstIndex[idx][1]][k].faceColors[tface] = tmp;
				}
			}
		}
	}
	
	// 获取需要与当前索引的值进行交换的索引，用于模拟旋转
	// outArr1 { [X1][Y1] [X2][Y2] ... }
	//              ||       ||
	// outArr2 { [X1][Y1] [X2][Y2] ... }
	this.getSwapIndexArray = function(minTimes)
	{
		let outArr = {};
		// 进行一次顺时针90度旋转相当逆时针交换6次(顶角和棱各3次)
		// 1   2   4   2   4   2   4   1
		//   *   ->  *   ->  *   ->  *
		// 4   3   1   3   3   1   3   2
		if (minTimes == 1)
		{
			outArr.srcIndex = [[0, 0], [0, 1], [0, 2], [1, 2], [2, 2], [2, 1]];
			outArr.dstIndex = [[0, 2], [1, 2], [2, 2], [2, 1], [2, 0], [1, 0]];
			outArr.size = 6;
		}
		// 进行一次顺时针90度旋转相当逆时针交换4次(顶角和棱各2次)
		// 1   2   3   2   3   4
		//   *   ->  *   ->  *  
		// 4   3   4   1   2   1
		else if (minTimes == 2)
		{
			outArr.srcIndex = [[0, 0], [0, 1], [0, 2], [1, 2]];
			outArr.dstIndex = [[2, 2], [2, 1], [2, 0], [1, 0]];
			outArr.size = 4;
		}
		// 进行一次顺时针90度旋转相当逆时针交换6次(顶角和棱各3次)
		// 1   2   4   2   4   2   4   1
		//   *   ->  *   ->  *   ->  *
		// 4   3   1   3   3   1   3   2
		else if (minTimes == 3)
		{
			outArr.srcIndex = [[0, 0], [1, 0], [2, 0], [2, 1], [2, 2], [1, 2]];
			outArr.dstIndex = [[2, 0], [2, 1], [2, 2], [1, 2], [0, 2], [0, 1]];
			outArr.size = 6;
		}
		// 0次顺时针旋转不变，其余异常数值也不变
		else
		{
			outArr.size = 0;
		}
		return outArr;
	}
	
	// 获取绕X轴旋转的情况下需要与目标索引块交换的面，用于模拟旋转
	// cube[][Y][Z].face1 <--> cube[][Y][Z].face2
	this.getTargetSwapFaceRotationX = function(face, times)
	{
		if (face == RubikFace_PosX || face == RubikFace_NegX)
			return face;
		while (times--)
		{
			switch (face)
			{
				case RubikFace_PosY: face = RubikFace_NegZ; break;
				case RubikFace_PosZ: face = RubikFace_PosY; break;
				case RubikFace_NegY: face = RubikFace_PosZ; break;
				case RubikFace_NegZ: face = RubikFace_NegY; break;
			}
		}
		return face;
	}
	
	// 获取绕Y轴旋转的情况下需要与目标索引块交换的面，用于模拟旋转
	// cube[X][][Z].face1 <--> cube[X][][Z].face2
	this.getTargetSwapFaceRotationY = function(face, times)
	{
		if (face == RubikFace_PosY || face == RubikFace_NegY)
			return face;
		while (times--)
		{
			switch (face)
			{
			case RubikFace_PosZ: face = RubikFace_NegX; break;
			case RubikFace_PosX: face = RubikFace_PosZ; break;
			case RubikFace_NegZ: face = RubikFace_PosX; break;
			case RubikFace_NegX: face = RubikFace_NegZ; break;
			}
		}
		return face;
	}
	// 获取绕Z轴旋转的情况下需要与目标索引块交换的面，用于模拟旋转
	// cube[X][Y][].face1 <--> cube[X][Y][].face2
	this.getTargetSwapFaceRotationZ = function(face, times)
	{
		if (face == RubikFace_PosZ || face == RubikFace_NegZ)
			return face;
		while (times--)
		{
			switch (face)
			{
			case RubikFace_PosX: face = RubikFace_NegY; break;
			case RubikFace_PosY: face = RubikFace_PosX; break;
			case RubikFace_NegX: face = RubikFace_PosY; break;
			case RubikFace_NegY: face = RubikFace_NegX; break;
			}
		}
		return face;
	}
}
