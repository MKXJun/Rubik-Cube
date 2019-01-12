#ifndef RUBIK_H
#define RUBIK_H

#include <wrl/client.h>
#include "Effects.h"
#include "Collision.h"
#include <vector>
#include <stack>

enum RubikFaceColor {
	RubikFaceColor_Black,		// 黑色
	RubikFaceColor_Orange,		// 橙色
	RubikFaceColor_Red,			// 红色
	RubikFaceColor_Green,		// 绿色
	RubikFaceColor_Blue,		// 蓝色
	RubikFaceColor_Yellow,		// 黄色
	RubikFaceColor_White		// 白色
};

enum RubikFace {
	RubikFace_PosX,		// +X面
	RubikFace_NegX,		// -X面
	RubikFace_PosY,		// +Y面
	RubikFace_NegY,		// -Y面
	RubikFace_PosZ,		// +Z面
	RubikFace_NegZ,		// -Z面
};

enum RubikRotationAxis {
	RubikRotationAxis_X,	// 绕X轴旋转
	RubikRotationAxis_Y,	// 绕Y轴旋转
	RubikRotationAxis_Z,	// 绕Z轴旋转
};

struct RubikRotationRecord
{
	RubikRotationAxis axis;	// 当前旋转轴
	int pos;				// 当前旋转层的索引
	float dTheta;			// 当前旋转的弧度
};

struct Cube
{
	// 获取当前立方体的世界矩阵
	DirectX::XMMATRIX GetWorldMatrix() const;

	RubikFaceColor faceColors[6];	// 六个面的颜色，索引0-5分别对应+X, -X, +Y, -Y, +Z, -Z面
	DirectX::XMFLOAT3 pos;			// 旋转结束后中心所处位置
	DirectX::XMFLOAT3 rotation;		// 仅允许存在单轴旋转，记录当前分别绕x轴, y轴, z轴旋转的弧度

};


class Rubik
{
public:
	template<class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;

	Rubik();

	// 初始化资源
	void InitResources(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext);
	// 立即复原魔方
	void Reset();
	// 更新魔方状态
	void Update(float dt);
	// 绘制魔方
	void Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect);
	// 当前是否在进行动画中
	bool IsLocked() const;
	// 当前魔方是否还原
	bool IsCompleted() const;


	// 当前射线拾取到哪个立方体(只考虑可见立方体)的对应索引，未找到则返回(-1, -1, -1)
	DirectX::XMINT3 HitCube(Ray ray, float * pDist = nullptr) const;



	// pos的取值为0-2时，绕X轴旋转魔方指定层 
	// pos的取值为-1时，绕X轴旋转魔方pos为0和1的两层
	// pos的取值为-2时，绕X轴旋转魔方pos为1和2的两层
	// pos的取值为3时，绕X轴旋转整个魔方
	void RotateX(int pos, float dTheta, bool isPressed = false);

	// pos的取值为3时，绕Y轴旋转魔方指定层 
	// pos的取值为-1时，绕Y轴旋转魔方pos为0和1的两层
	// pos的取值为-2时，绕Y轴旋转魔方pos为1和2的两层
	// pos的取值为3时，绕Y轴旋转整个魔方
	void RotateY(int pos, float dTheta, bool isPressed = false);

	// pos的取值为0-2时，绕Z轴旋转魔方指定层 
	// pos的取值为-1时，绕Z轴旋转魔方pos为0和1的两层
	// pos的取值为-2时，绕Z轴旋转魔方pos为1和2的两层
	// pos的取值为3时，绕Z轴旋转整个魔方
	void RotateZ(int pos, float dTheta, bool isPressed = false);
	
	
	

	// 设置旋转速度(rad/s)
	void SetRotationSpeed(float rad);

	// 获取纹理数组
	ComPtr<ID3D11ShaderResourceView> GetTexArray() const;

private:
	// 从内存中创建纹理
	ComPtr<ID3D11ShaderResourceView> CreateRubikCubeTextureArrayFromMemory(ComPtr<ID3D11Device> device,
		ComPtr<ID3D11DeviceContext> deviceContext);

	// 绕X轴的预旋转
	void PreRotateX(bool isKeyOp);
	// 绕Y轴的预旋转
	void PreRotateY(bool isKeyOp);
	// 绕Z轴的预旋转
	void PreRotateZ(bool isKeyOp);

	// 获取需要与当前索引的值进行交换的索引，用于模拟旋转
	// outArr1 { [X1][Y1] [X2][Y2] ... }
	//              ||       ||
	// outArr2 { [X1][Y1] [X2][Y2] ... }
	void GetSwapIndexArray(int times, std::vector<DirectX::XMINT2>& outArr1, 
		std::vector<DirectX::XMINT2>& outArr2) const;

	// 获取绕X轴旋转的情况下需要与目标索引块交换的面，用于模拟旋转
	// cube[][Y][Z].face1 <--> cube[][Y][Z].face2
	RubikFace GetTargetSwapFaceRotationX(RubikFace face, int times) const;
	// 获取绕Y轴旋转的情况下需要与目标索引块交换的面，用于模拟旋转
	// cube[X][][Z].face1 <--> cube[X][][Z].face2
	RubikFace GetTargetSwapFaceRotationY(RubikFace face, int times) const;
	// 获取绕Z轴旋转的情况下需要与目标索引块交换的面，用于模拟旋转
	// cube[X][Y][].face1 <--> cube[X][Y][].face2
	RubikFace GetTargetSwapFaceRotationZ(RubikFace face, int times) const;

private:
	// 魔方 [X][Y][Z]
	Cube mCubes[3][3][3];

	// 当前是否鼠标正在拖动
	bool mIsPressed;
	// 当前是否有动画在播放
	bool mIsLocked;
	// 当前自动旋转的速度
	float mRotationSpeed;

	// 顶点缓冲区，包含6个面的24个顶点
	// 索引0-3对应+X面
	// 索引4-7对应-X面
	// 索引8-11对应+Y面
	// 索引12-15对应-Y面
	// 索引16-19对应+Z面
	// 索引20-23对应-Z面
	ComPtr<ID3D11Buffer> mVertexBuffer;	

	// 索引缓冲区，仅6个索引
	ComPtr<ID3D11Buffer> mIndexBuffer;
	
	// 实例缓冲区，

	// 纹理数组，包含7张纹理
	ComPtr<ID3D11ShaderResourceView> mTexArray;
};




#endif