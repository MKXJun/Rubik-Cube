#ifndef GAMEAPP_H
#define GAMEAPP_H

#include "d3dApp.h"
#include "Rubik.h"

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

private:
	ComPtr<ID2D1SolidColorBrush> mColorBrush;	// 单色笔刷
	ComPtr<IDWriteFont> mFont;					// 字体
	ComPtr<IDWriteTextFormat> mTextFormat;		// 文本格式

	Rubik mRubik;								// 魔方

	BasicEffect mBasicEffect;					// 基础特效管理类
};


#endif