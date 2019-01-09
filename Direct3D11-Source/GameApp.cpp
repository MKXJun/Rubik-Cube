#include "GameApp.h"
#include "d3dUtil.h"
using namespace DirectX;
using namespace std::experimental;

GameApp::GameApp(HINSTANCE hInstance)
	: D3DApp(hInstance)
{
}

GameApp::~GameApp()
{
}

bool GameApp::Init()
{
	if (!D3DApp::Init())
		return false;

	if (!mBasicEffect.InitAll(md3dDevice))
		return false;

	if (!InitResource())
		return false;

	// 初始化鼠标，键盘不需要
	mMouse->SetWindow(mhMainWnd);
	mMouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);

	// 初始化滑动延迟时间
	mSlideDelay = 0.05f;

	return true;
}

void GameApp::OnResize()
{
	assert(md2dFactory);
	assert(mdwriteFactory);
	// 释放D2D的相关资源
	mColorBrush.Reset();
	md2dRenderTarget.Reset();

	D3DApp::OnResize();

	// 为D2D创建DXGI表面渲染目标
	ComPtr<IDXGISurface> surface;
	HR(mSwapChain->GetBuffer(0, __uuidof(IDXGISurface), reinterpret_cast<void**>(surface.GetAddressOf())));
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED));
	HRESULT hr = md2dFactory->CreateDxgiSurfaceRenderTarget(surface.Get(), &props, md2dRenderTarget.GetAddressOf());
	surface.Reset();

	if (hr == E_NOINTERFACE)
	{
		OutputDebugString(L"\n警告：Direct2D与Direct3D互操作性功能受限，你将无法看到文本信息。现提供下述可选方法：\n"
			"1. 对于Win7系统，需要更新至Win7 SP1，并安装KB2670838补丁以支持Direct2D显示。\n"
			"2. 自行完成Direct3D 10.1与Direct2D的交互。详情参阅："
			"https://docs.microsoft.com/zh-cn/windows/desktop/Direct2D/direct2d-and-direct3d-interoperation-overview""\n"
			"3. 使用别的字体库，比如FreeType。\n\n");
	}
	else if (hr == S_OK)
	{
		// 创建固定颜色刷和文本格式
		HR(md2dRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			mColorBrush.GetAddressOf()));
		HR(mdwriteFactory->CreateTextFormat(L"宋体", nullptr, DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 20, L"zh-cn",
			mTextFormat.GetAddressOf()));
	}
	else
	{
		// 报告异常问题
		assert(md2dRenderTarget);
	}

	// 摄像机变更显示
	if (mCamera != nullptr)
	{
		mCamera->SetFrustum(XM_PI * 0.4f, AspectRatio(), 1.0f, 1000.0f);
		mCamera->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
		mBasicEffect.SetProjMatrix(mCamera->GetProjXM());
	}
}

void GameApp::UpdateScene(float dt)
{
	KeyInput(dt);
	MouseInput(dt);
	

	// 反复旋转
	static float theta = XM_PIDIV2;
	if (!mRubik.IsLocked())
	{
		theta *= -1.0f;
	}
	// 就算摆出来也不会有问题(只有未上锁的帧才会生效该调用)
	//mRubik.RotateZ(theta);
	//// 下面的也不会被调用
	//mRubik.RotateX(2, theta);
	//mRubik.RotateY(0, theta);
	
	// 更新魔方
	mRubik.Update(dt);

}

void GameApp::DrawScene()
{
	assert(md3dImmediateContext);
	assert(mSwapChain);

	// 使用偏紫色的纯色背景
	float backgroundColor[4] = { 0.45882352f, 0.42745098f, 0.51372549f, 1.0f };
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView.Get(), backgroundColor);
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	
	// 绘制魔方
	mRubik.Draw(md3dImmediateContext, mBasicEffect);


	//
	// 绘制Direct2D部分
	//
	if (md2dRenderTarget != nullptr)
	{
		md2dRenderTarget->BeginDraw();

		// 用于Debug输出
		Mouse::State mouseState = mMouse->GetState();
		std::wstring wstr = L"鼠标位置：(" + std::to_wstring(mouseState.x) + L", " + std::to_wstring(mouseState.y) + L")";
		Ray ray = Ray::ScreenToRay(*mCamera, (float)mouseState.x, (float)mouseState.y);
		float dist;
		XMINT3 pos = mRubik.HitCube(ray, &dist);
		wstr += L"\n选中方块索引：[" + std::to_wstring(pos.x) + L"][" + std::to_wstring(pos.y) + L"][" + std::to_wstring(pos.z) + L"]\n"
			"击中点：";
		if (dist == 0.0f)
		{
			wstr += L"无";
		}
		else
		{
			wstr += L"(" + std::to_wstring(ray.origin.x + dist * ray.direction.x) + L", " +
				std::to_wstring(ray.origin.y + dist * ray.direction.y) + L", " +
				std::to_wstring(ray.origin.z + dist * ray.direction.z) + L")";
		}

		md2dRenderTarget->DrawTextW(wstr.c_str(), (UINT)wstr.size(), mTextFormat.Get(),
			D2D1_RECT_F{ 0.0f, 0.0f, 600.0f, 200.0f }, mColorBrush.Get());
		HR(md2dRenderTarget->EndDraw());
	}

	HR(mSwapChain->Present(0, 0));
}



bool GameApp::InitResource()
{
	// 初始化魔方
	mRubik.InitResources(md3dDevice, md3dImmediateContext);
	mRubik.SetRotationSpeed(XM_2PI * 1.5f);
	// 初始化特效
	mBasicEffect.SetRenderDefault(md3dImmediateContext);
	// 初始化摄像机
	mCamera.reset(new ThirdPersonCamera);
	auto camera3rd = dynamic_cast<ThirdPersonCamera*>(mCamera.get());
	camera3rd->SetDistance(10.0f);
	camera3rd->SetDistanceMinMax(10.0f, 20.0f);
	camera3rd->SetFrustum(XM_PI * 0.4f, AspectRatio(), 1.0f, 1000.0f);
	camera3rd->SetViewPort(0.0f, 0.0f, (float)mClientWidth, (float)mClientHeight);
	camera3rd->SetTarget(XMFLOAT3());
	mBasicEffect.SetProjMatrix(camera3rd->GetProjXM());
	mBasicEffect.SetTextureArray(mRubik.GetTexArray());
	

	return true;
}

void GameApp::KeyInput(float dt)
{
	Keyboard::State keyState = mKeyboard->GetState();
	mKeyboardTracker.Update(keyState);

	//
	// 整个魔方旋转
	//

	// 公式x
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Up))
		mRubik.RotateX(XM_PIDIV2);
	// 公式x'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Down))
		mRubik.RotateX(-XM_PIDIV2);
	// 公式y
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Left))
		mRubik.RotateY(XM_PIDIV2);
	// 公式y'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Right))
		mRubik.RotateY(-XM_PIDIV2);
	// 公式z'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::PageUp))
		mRubik.RotateZ(XM_PIDIV2);
	// 公式z
	if (mKeyboardTracker.IsKeyPressed(Keyboard::PageDown))
		mRubik.RotateZ(-XM_PIDIV2);

	//
	// 单层旋转
	//

	// 公式R
	if (mKeyboardTracker.IsKeyPressed(Keyboard::I))
		mRubik.RotateX(2, XM_PIDIV2);
	// 公式R'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::K))
		mRubik.RotateX(2, -XM_PIDIV2);
	// 公式U
	if (mKeyboardTracker.IsKeyPressed(Keyboard::J))
		mRubik.RotateY(2, XM_PIDIV2);
	// 公式U'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::L))
		mRubik.RotateY(2, -XM_PIDIV2);
	// 公式F'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::U))
		mRubik.RotateZ(0, XM_PIDIV2);
	// 公式F
	if (mKeyboardTracker.IsKeyPressed(Keyboard::O))
		mRubik.RotateZ(0, -XM_PIDIV2);

	// 公式L'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::W))
		mRubik.RotateX(0, XM_PIDIV2);
	// 公式L
	if (mKeyboardTracker.IsKeyPressed(Keyboard::S))
		mRubik.RotateX(0, -XM_PIDIV2);
	// 公式D'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::A))
		mRubik.RotateY(0, XM_PIDIV2);
	// 公式D
	if (mKeyboardTracker.IsKeyPressed(Keyboard::D))
		mRubik.RotateY(0, -XM_PIDIV2);
	// 公式B
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Q))
		mRubik.RotateZ(2, XM_PIDIV2);
	// 公式B'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::E))
		mRubik.RotateZ(2, -XM_PIDIV2);

	// 公式M
	if (mKeyboardTracker.IsKeyPressed(Keyboard::T))
		mRubik.RotateX(1, XM_PIDIV2);
	// 公式M'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::G))
		mRubik.RotateX(1, -XM_PIDIV2);
	// 公式E
	if (mKeyboardTracker.IsKeyPressed(Keyboard::F))
		mRubik.RotateY(1, XM_PIDIV2);
	// 公式E'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::H))
		mRubik.RotateY(1, -XM_PIDIV2);
	// 公式S'
	if (mKeyboardTracker.IsKeyPressed(Keyboard::R))
		mRubik.RotateZ(1, XM_PIDIV2);
	// 公式S
	if (mKeyboardTracker.IsKeyPressed(Keyboard::Y))
		mRubik.RotateZ(1, -XM_PIDIV2);

	
}

void GameApp::MouseInput(float dt)
{
	Mouse::State mouseState = mMouse->GetState();
	Mouse::State lastState = mMouseTracker.GetLastState();
	mMouseTracker.Update(mouseState);
	

	int dx = mouseState.x - lastState.x;
	int dy = mouseState.y - lastState.y;
	// 获取子类
	auto cam3rd = dynamic_cast<ThirdPersonCamera*>(mCamera.get());

	// ******************
	// 第三人称摄像机的操作
	//

	// 绕物体旋转，添加轻微抖动
	cam3rd->SetRotationX(XM_PIDIV2 * 0.6f + (mouseState.y - mClientHeight / 2) *  0.0001f);
	cam3rd->SetRotationY(-XM_PIDIV4 + (mouseState.x - mClientWidth / 2) * 0.0001f);
	cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);

	// 更新观察矩阵
	mCamera->UpdateViewMatrix();
	mBasicEffect.SetViewMatrix(mCamera->GetViewXM());

	// 重置滚轮值
	mMouse->ResetScrollWheelValue();

	// ******************
	// 魔方操作
	//

	// 鼠标左键是否点击
	if (mouseState.leftButton)
	{
		// 此时未确定旋转方向
		if (!mDirectionLocked)
		{
			// 此时未记录点击位置
			if (mClickPosX == -1 && mClickPosY == -1)
			{
				// 初次点击
				if (mMouseTracker.leftButton == Mouse::ButtonStateTracker::PRESSED)
				{
					// 记录点击位置
					mClickPosX = mouseState.x;
					mClickPosY = mouseState.y;
				}
			}
			
			// 仅当记录了点击位置才进行更新
			if (mClickPosX != -1 && mClickPosY != -1)
				mCurrDelay += dt;
			// 未到达滑动延迟时间则结束
			if (mCurrDelay < mSlideDelay)
				return;

			// 未产生运动则不上锁
			if (abs(dx) == abs(dy))
				return;

			// 开始上方向锁
			mDirectionLocked = true;
			// 更新累积的位移变化量
			dx = mouseState.x - mClickPosX;
			dy = mouseState.y - mClickPosY;

			// 找到当前鼠标点击的方块索引
			Ray ray = Ray::ScreenToRay(*mCamera, (float)mouseState.x, (float)mouseState.y);
			float dist;
			XMINT3 pos = mRubik.HitCube(ray, &dist);

			// 判断当前主要是垂直操作还是水平操作
			bool isVertical = abs(dx) < abs(dy);
			// 当前鼠标操纵的是-Z面，根据操作类型决定旋转轴
			if (pos.z == 0 && fabs((ray.origin.z + dist * ray.direction.z) - (-3.0f)) < 1e-5f)
			{
				mSlidePos = isVertical ? pos.x : pos.y;
				mCurrRotationAxis = isVertical ? RubikRotationAxis_X : RubikRotationAxis_Y;
			}
			// 当前鼠标操纵的是+X面，根据操作类型决定旋转轴
			else if (pos.x == 2 && fabs((ray.origin.x + dist * ray.direction.x) - 3.0f) < 1e-5f)
			{
				mSlidePos = isVertical ? pos.z : pos.y;
				mCurrRotationAxis = isVertical ? RubikRotationAxis_Z : RubikRotationAxis_Y;
			}
			// 当前鼠标操纵的是+Y面，要判断平移变化量dx和dy的符号来决定旋转方向
			else if (pos.y == 2 && fabs((ray.origin.y + dist * ray.direction.y) - 3.0f) < 1e-5f)
			{
				// 判断异号
				bool diffSign = ((dx & 0x80000000) != (dy & 0x80000000));
				mSlidePos = diffSign ? pos.x : pos.z;
				mCurrRotationAxis = diffSign ? RubikRotationAxis_X : RubikRotationAxis_Z;
			}
			// 当前鼠标操纵的是空白地区，则对整个魔方旋转
			else
			{
				mSlidePos = -1;
				// 水平操作是Y轴旋转
				if (!isVertical)
				{
					mCurrRotationAxis = RubikRotationAxis_Y;
				}
				// 屏幕左半部分的垂直操作是X轴旋转
				else if (mouseState.x < mClientWidth / 2)
				{
					mCurrRotationAxis = RubikRotationAxis_X;
				}
				// 屏幕右半部分的垂直操作是Z轴旋转
				else
				{
					mCurrRotationAxis = RubikRotationAxis_Z;
				}
			}
		}

		// 上了方向锁才能进行旋转
		if (mDirectionLocked)
		{
			// 进行旋转
			if (mSlidePos == -1)
			{
				switch (mCurrRotationAxis)
				{
				case RubikRotationAxis_X: mRubik.RotateX((dx - dy) * 0.01f, true); break;
				case RubikRotationAxis_Y: mRubik.RotateY(-dx * 0.01f, true); break;
				case RubikRotationAxis_Z: mRubik.RotateZ((-dx - dy) * 0.01f, true); break;
				}
			}
			else
			{
				switch (mCurrRotationAxis)
				{
				case RubikRotationAxis_X: mRubik.RotateX(mSlidePos, (dx - dy) * 0.01f, true); break;
				case RubikRotationAxis_Y: mRubik.RotateY(mSlidePos, -dx * 0.01f, true); break;
				case RubikRotationAxis_Z: mRubik.RotateZ(mSlidePos, (-dx - dy) * 0.01f, true); break;
				}

			}
		}
	}
	// 鼠标刚释放
	else if (mMouseTracker.leftButton == Mouse::ButtonStateTracker::RELEASED)
	{
		// 释放方向锁
		mDirectionLocked = false;
		// 滑动延迟归零
		mCurrDelay = 0.0f;
		// 坐标移出屏幕
		mClickPosX = mClickPosY = -1;
		// 发送完成指令，进行预旋转
		if (mSlidePos == -1)
		{
			switch (mCurrRotationAxis)
			{
			case RubikRotationAxis_X: mRubik.RotateX(0.0f); break;
			case RubikRotationAxis_Y: mRubik.RotateY(0.0f); break;
			case RubikRotationAxis_Z: mRubik.RotateZ(0.0f); break;
			}
		}
		else
		{
			switch (mCurrRotationAxis)
			{
			case RubikRotationAxis_X: mRubik.RotateX(mSlidePos, 0.0f); break;
			case RubikRotationAxis_Y: mRubik.RotateY(mSlidePos, 0.0f); break;
			case RubikRotationAxis_Z: mRubik.RotateZ(mSlidePos, 0.0f); break;
			}

		}
	}
}

