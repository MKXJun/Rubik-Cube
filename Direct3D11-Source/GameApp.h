#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "d3dApp.h"
#include "Rubik.h"
#include "Camera.h"

class GameApp : public D3DApp
{
public:
	GameApp(HINSTANCE hInstance);
	~GameApp();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();


private:
	bool InitResource();

	void KeyInput();
	void MouseInput(float dt);

private:
	ComPtr<ID2D1SolidColorBrush> mColorBrush;	// 单色笔刷
	ComPtr<IDWriteFont> mFont;					// 字体
	ComPtr<IDWriteTextFormat> mTextFormat;		// 文本格式

	Rubik mRubik;								// 魔方

	//
	// 鼠标操作控制
	//
	
	int mClickPosX, mClickPosY;					// 初次点击时鼠标位置
	float mSlideDelay;							// 拖动延迟响应时间 
	float mCurrDelay;							// 当前延迟时间
	bool mDirectionLocked;						// 方向锁
	RubikRotationAxis mCurrRotationAxis;		// 当前鼠标拖动时的旋转轴
	int mSlidePos;								// 当前鼠标拖动的层数索引，-1为整个魔方

	std::unique_ptr<Camera> mCamera;			// 第三人称摄像机

	BasicEffect mBasicEffect;					// 基础特效管理类
};


#endif