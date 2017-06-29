#include "DirectX.h"
using namespace std;

string APPTITLE = "魔方";
bool gameover = false;


LRESULT WINAPI WinProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		gameover = true;
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand((unsigned)time(0));
	//初始化窗口设定
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WinProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = APPTITLE.c_str();
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	//新建窗口句柄，固定窗口大小方便鼠标定位
	HWND window = CreateWindow(APPTITLE.c_str(), APPTITLE.c_str(),
		WS_OVERLAPPED | WS_SYSMENU, 600, 200,
		SCREENW, SCREENH, nullptr, nullptr, hInstance, nullptr);
	if (window == nullptr) return 0;

	//显示窗口
	ShowWindow(window, nCmdShow);
	UpdateWindow(window);

	//Direct3D初始化
	if (!Game_Init(window))
	{
		MessageBox(nullptr, "Initializing Direct3D Failed.", "Error", MB_OK | MB_ICONEXCLAMATION);
		gameover = true;
	}

	//信息处理循环
	MSG message;
	while (!gameover)
	{
		Game_Run(window);
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}

	}

	//释放Direct3D相关
	Game_End();

	return message.wParam;
}
