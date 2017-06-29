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
#define SCREENW 600		//屏幕宽度
#define SCREENH 600		//屏幕高度

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"dxguid.lib")
#define D3DFVF_MYVETREX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#define KEY_DOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)


struct Vetrex
{
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	float u, v;
	Vetrex(){}
	Vetrex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
		: pos(x,y,z), normal(nx, ny, nz), u(u), v(v)
	{
	}
};

// DirectX部分
LPDIRECT3DTEXTURE9 LoadTexture(const char * filename);


void WorldTransform();
void ViewTransform(D3DXVECTOR3 eye, D3DXVECTOR3 lookAt, D3DXVECTOR3 up);
void PerspectiveTransform(float fieldOfView, float aspectRatio, float nearRange, float farRange);


bool Direct3D_Init(HWND window);
void Direct3D_Shutdown();

//DirectInput部分
bool DirectInput_Init(HWND window);
void DirectInput_Update();
void DirectInput_Shutdown();
bool Key_Down(int key);

//数学部分
double MySin(double _X);
double MyCos(double _X);
//游戏部分
bool Game_Init(HWND window);
void Game_Run(HWND window);
void Game_End();

void Key_Update();
//带精度浮点数转换
std::string to_string(double _Val, int precision);