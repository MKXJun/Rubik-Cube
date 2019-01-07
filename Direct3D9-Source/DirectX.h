#pragma once
#define WIN32_EXTRA_LEAN
#define DIRECTINPUT_VERSION 0x0800
#include <d3dx9.h>
#include <d3d9.h>
#include <ctime>
#include <Windows.h>
#include <string>
#include <dinput.h>
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <stack>
#include <functional>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"dxguid.lib")

//定义区
#define D3DFVF_MYVETREX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define SCREENW 600		//屏幕宽度
#define SCREENH 600		//屏幕高度

using std::stack;
using std::function;

struct Vetrex
{
	D3DXVECTOR3 pos;		//位置坐标
	D3DXVECTOR3 normal;		//法线
	float u, v;				//纹理坐标
	Vetrex()
		: pos{}, normal{}, u{}, v{}
	{
	}

	Vetrex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: pos(x,y,z), normal(nx, ny, nz), u(u), v(v)
	{
	}
};

extern LPDIRECT3D9 d3d;							//D3D9接口
extern LPDIRECT3DDEVICE9 d3ddev;				//D3D9设备
extern D3DPRESENT_PARAMETERS d3dpp;				//D3D参数


extern LPDIRECTINPUT8 dinput;					//输入接口
extern LPDIRECTINPUTDEVICE8 dikeyboard;			//键盘接口
extern char keys[256];							//记录按键状态
extern LPDIRECTINPUTDEVICE8 dimouse;			//鼠标接口
extern DIMOUSESTATE mouse_state;				//鼠标状态
extern bool slide;								//鼠标滑动中


extern LPDIRECT3DTEXTURE9 red, yellow, blue,
green, white, orange, black;		//魔方面颜色

extern LPDIRECT3DVERTEXBUFFER9 vb;				//D3D顶点缓存
extern LPDIRECT3DINDEXBUFFER9 ib;				//D3D顶点缓存

extern LPD3DXFONT font;							//字体
extern RECT font_rect;							//输出矩形
extern RECT window_rect;						//窗体矩形
extern POINT mouse_pos;							//鼠标位置
extern POINT click_pos;							//点击位置

extern DWORD start_time;						//游戏开始的时间
extern DWORD finish_time;						//完成魔方的时间
extern DWORD current_time;						//计时器
extern DWORD frameCnt;							//帧计数器
extern DWORD fps;								//帧每秒				
extern DWORD delay;								//一帧间隔
extern DWORD frame_timer;						//帧计时器
extern DWORD key_down_counter;					//按键间隔计数，18帧一按
extern DWORD slide_delay;						//滑动延迟

extern bool use_pick;							//为true时使用动态拾取，false时使用静态描点

//照相机初始位置，朝向目标，摄影机上视角
extern D3DXVECTOR3 pos;
extern D3DXVECTOR3 target;
extern D3DXVECTOR3 up;

extern D3DMATERIAL9 material;					//纹理材质

extern D3DLIGHT9 light;							//灯光

extern stack<function<void()>> func_stk;		//函数栈，保存了魔方逆操作

//函数区
// DirectX部分
LPDIRECT3DTEXTURE9 LoadTexture(const char * filename);


void WorldTransform();
void ViewTransform(const D3DXVECTOR3& eye, const D3DXVECTOR3& lookAt, const D3DXVECTOR3& up);
void PerspectiveTransform(float fieldOfView, float aspectRatio, float nearRange, float farRange);


bool Direct3D_Init(HWND window);
void Direct3D_Reset(HWND window);
void Direct3D_Shutdown();

//DirectInput部分
bool DirectInput_Init(HWND window);
void DirectInput_Update();
void DirectInput_Shutdown();
bool Key_Down(int key);
bool Mouse_Click(int button);

//数学部分
double MySin(double _X);
double MyCos(double _X);
//游戏部分
bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();
void PreView();


void Key_Update(HWND window);
void Mouse_Update(HWND window);

bool Click_Rect(const POINT& LeftBottom, const POINT& LeftTop, const POINT& RightTop, const POINT& RightBottom);
bool IsPick(HWND window, const Vetrex& LeftBottom, const Vetrex& LeftTop, const Vetrex& RightTop, const Vetrex& RightBottom);
//自定义带精度浮点数转换（只能在VS2017中使用）
_STD_BEGIN
std::string to_string(float _Val, int precision);
_STD_END