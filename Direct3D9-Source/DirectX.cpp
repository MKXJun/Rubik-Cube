#include "DirectX.h"
//变量声明区
LPDIRECT3D9 d3d;													
LPDIRECT3DDEVICE9 d3ddev;											
D3DPRESENT_PARAMETERS d3dpp;

LPDIRECTINPUT8 dinput;												
LPDIRECTINPUTDEVICE8 dikeyboard;									
char keys[256];
LPDIRECTINPUTDEVICE8 dimouse;										
DIMOUSESTATE mouse_state;		
bool slide = false;

LPDIRECT3DTEXTURE9 red, yellow, blue, green, white, orange, black;	

LPDIRECT3DVERTEXBUFFER9 vb;											
LPDIRECT3DINDEXBUFFER9 ib;											

LPD3DXFONT font;													
RECT font_rect = { 400, 0, 600, 50 };								
RECT window_rect;													
POINT mouse_pos;
POINT click_pos;

DWORD start_time;
DWORD finish_time;
DWORD current_time = timeGetTime();									
DWORD frameCnt = 0;
DWORD fps = 0;
DWORD delay = 8;													
DWORD frame_timer = timeGetTime();									
DWORD key_down_counter;												
DWORD slide_delay = 0;

float preview_rotation = 0.0f;


bool use_pick = true;

D3DXVECTOR3 pos(12.0f, 12.0f, -12.0f);
D3DXVECTOR3 target(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 up(-1.0f, 8.0f, 1.0f);

D3DMATERIAL9 material;

D3DLIGHT9 light;

stack<function<void()>> func_stk;

//创建纹理
LPDIRECT3DTEXTURE9 LoadTexture(const char * filename)
{
	LPDIRECT3DTEXTURE9 texture = nullptr;
	D3DXCreateTextureFromFile(d3ddev, filename, &texture);
	if (!texture)
		return nullptr;
	return texture;
}

// 世界变换
void WorldTransform()
{
	D3DXMATRIX matWorld;
	D3DXMatrixTranslation(&matWorld, 0.0f, 0.0f, 0.0f);
	d3ddev->SetTransform(D3DTS_WORLD, &matWorld);
}

// 取景变换，eye为摄影机位置，lookAt为摄影机朝向，up为摄影机上方向
void ViewTransform(const D3DXVECTOR3& eye,const D3DXVECTOR3& lookAt,const D3DXVECTOR3& up)
{
	D3DXMATRIX View;
	D3DXMatrixLookAtLH(&View, &eye, &lookAt, &up);
	d3ddev->SetTransform(D3DTS_VIEW, &View);
}

// 投影变换，fieldOfView为垂直视野角度，aspectRatio为宽高比，nearRange为前裁减面距离，farRange为后裁减面距离
void PerspectiveTransform(float fieldOfView, float aspectRatio, float nearRange, float farRange)
{
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, fieldOfView, aspectRatio, nearRange, farRange);
	d3ddev->SetTransform(D3DTS_PROJECTION, &proj);
}

//Direct3D接口与设备初始化
bool Direct3D_Init(HWND window)
{
	//创建D3D9对象
	d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!d3d)
		return false;
	//初始化输入设备
	if (!DirectInput_Init(window))
		return false;

	D3DDISPLAYMODE displaymode;
	if (FAILED(d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displaymode)))
		return false;

	//填充D3D参数
	
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.BackBufferWidth = SCREENW;		//后备缓冲区宽度
	d3dpp.BackBufferHeight = SCREENH;		//后备缓冲区高度
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;				//后备缓冲区数目
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed = true;		//窗口化
	d3dpp.hDeviceWindow = window;
	d3dpp.EnableAutoDepthStencil = 1;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality = 0;

	//检测硬件是否支持4倍抗锯齿，支持的话直接使用
	if (d3d->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, displaymode.Format, true, D3DMULTISAMPLE_4_SAMPLES, nullptr) == D3D_OK)
		d3dpp.MultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
	
	//创建设备
	d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,window,D3DCREATE_HARDWARE_VERTEXPROCESSING,&d3dpp,&d3ddev);
	
	if (!d3ddev)
		return false;

	

	//顶点和索引缓冲区初始化，由于顶点索引是固定的，在此一并初始化
	d3ddev->CreateVertexBuffer(648 * sizeof(Vetrex),D3DUSAGE_WRITEONLY,D3DFVF_MYVETREX,D3DPOOL_MANAGED,&vb,0);
	d3ddev->CreateIndexBuffer(972 * sizeof(WORD),D3DUSAGE_WRITEONLY,D3DFMT_INDEX16,D3DPOOL_MANAGED,&ib,0);

	WORD* indices;
	ib->Lock(0, 0, (void**)&indices, 0);

	for (int i = 0; i < 162; ++i)
	{
		indices[i * 6 + 0] = 4 * i + 0;
		indices[i * 6 + 1] = 4 * i + 1;
		indices[i * 6 + 2] = 4 * i + 2;
		indices[i * 6 + 3] = 4 * i + 2;
		indices[i * 6 + 4] = 4 * i + 3;
		indices[i * 6 + 5] = 4 * i + 0;
	}
	ib->Unlock();


	//设置过滤器
	d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//初始化字体
	D3DXFONT_DESC desc = { 30,0,0,0,false,DEFAULT_CHARSET,OUT_TT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_PITCH,"Arial" };
	D3DXCreateFontIndirect(d3ddev, &desc, &font);

	//纹理读取
	red = LoadTexture("Resource/Red.png");
	green = LoadTexture("Resource/Green.png");
	yellow = LoadTexture("Resource/Yellow.png");
	blue = LoadTexture("Resource/Blue.png");
	white = LoadTexture("Resource/White.png");
	orange = LoadTexture("Resource/Orange.png");
	black = LoadTexture("Resource/Black.png");
	
	//纹理材质
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Emissive = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	material.Power = 5.0f;

	d3ddev->SetMaterial(&material);

	//灯光初始化
	ZeroMemory(&light, sizeof(light));
	light.Type = D3DLIGHT_DIRECTIONAL;
	light.Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	light.Direction = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	d3ddev->SetLight(0, &light);
	d3ddev->LightEnable(0, true);
	d3ddev->SetRenderState(D3DRS_LIGHTING, true);
	d3ddev->SetRenderState(D3DRS_NORMALIZENORMALS, true);


	return true;
}

void Direct3D_Reset(HWND window)
{
	if (d3ddev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		font->OnLostDevice();
		d3ddev->Reset(&d3dpp);
		font->OnResetDevice();
		WorldTransform();
		ViewTransform(pos, target, up);
		PerspectiveTransform(D3DX_PI * 0.4f, (float)SCREENW / (float)SCREENH, 1.0f, 1000.0f);

		d3ddev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		d3ddev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		d3ddev->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

		d3ddev->SetMaterial(&material);

		d3ddev->SetLight(0, &light);
		d3ddev->LightEnable(0, true);
		d3ddev->SetRenderState(D3DRS_LIGHTING, true);
		d3ddev->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	}
}

//释放Direct3D相关
void Direct3D_Shutdown()
{
	if (d3ddev) d3ddev->Release();
	if (d3d) d3d->Release();
	if (vb) vb->Release();

	if (red) red->Release();
	if (green) green->Release();
	if (blue) blue->Release();
	if (white) white->Release();
	if (yellow) yellow->Release();
	if (orange) orange->Release();
	if (black) black->Release();

	if (font) font->Release();

	DirectInput_Shutdown();
}

//DirectInput初始化
bool DirectInput_Init(HWND window)
{
	//Direct输入对象创建
	DirectInput8Create(GetModuleHandle(nullptr), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&dinput, nullptr);

	//初始化键盘
	dinput->CreateDevice(GUID_SysKeyboard, &dikeyboard, nullptr);
	dikeyboard->SetDataFormat(&c_dfDIKeyboard);
	dikeyboard->SetCooperativeLevel(window, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dikeyboard->Acquire();

	//初始化鼠标
	dinput->CreateDevice(GUID_SysMouse, &dimouse, nullptr);
	dimouse->SetDataFormat(&c_dfDIMouse);
	dimouse->SetCooperativeLevel(window, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	dimouse->Acquire();
	return true;
}

//DirectInput更新
void DirectInput_Update()
{
	//更新键盘
	dikeyboard->Poll();
	if (!SUCCEEDED(dikeyboard->GetDeviceState(256, (LPVOID)&keys)))
	{
		//键盘设备丢失，尝试重新获取
		dikeyboard->Acquire();
	}

	//更新鼠标
	dimouse->Poll();
	if (!SUCCEEDED(dimouse->GetDeviceState(sizeof(DIMOUSESTATE), &mouse_state)))
	{
		//鼠标设备丢失，尝试重新获取
		dimouse->Acquire();
	}
}

//DirectInput释放
void DirectInput_Shutdown()
{
	if (dikeyboard)
	{
		dikeyboard->Unacquire();
		dikeyboard->Release();
		dikeyboard = nullptr;
	}
	if (dimouse)
	{
		dimouse->Unacquire();
		dimouse->Release();
		dimouse = nullptr;
	}
	dinput->Release();
}

//检测键盘按键
bool Key_Down(int key)
{
	return (bool)(keys[key] & 0x80);
}

//检测鼠标按键，0为鼠标左键
bool Mouse_Click(int button)
{
	return mouse_state.rgbButtons[button] & 0x80;
}

//处理sin(D3DX_PI)结果不为0的情况
double MySin(double _X)
{
	double res = sin(_X);
	return fabs(res) < 10e-5 ? 0.0 : res;
}

//处理cos(D3DX_PI / 2)结果不为0的情况
double MyCos(double _X)
{
	double res = cos(_X);
	return fabs(res) < 10e-5 ? 0.0 : res;
}


