#include "Rubik.h"

#include <algorithm>
extern LPDIRECT3DVERTEXBUFFER9 vb;
extern LPDIRECT3DINDEXBUFFER9 ib;
extern LPDIRECT3DDEVICE9 d3ddev;
extern LPDIRECT3DTEXTURE9 red, yellow, blue, green, white, orange, black;

extern DWORD key_down_counter;
extern DWORD start_time;
//魔方
Rubik rubik;


//重置魔方
void Rubik::reset()
{
	//初始化顶点的坐标、法向量和纹理坐标，方块所有面颜色先全涂黑
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				//顶面
				cube[i][j][k].v[0] = Vetrex(-3.0f + 2.0f * i, -1.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
				cube[i][j][k].v[1] = Vetrex(-3.0f + 2.0f * i, -1.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
				cube[i][j][k].v[2] = Vetrex(-1.0f + 2.0f * i, -1.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
				cube[i][j][k].v[3] = Vetrex(-1.0f + 2.0f * i, -1.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);
				cube[i][j][k].top = black;

				//底面
				cube[i][j][k].v[4] = Vetrex(-1.0f + 2.0f * i, -3.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
				cube[i][j][k].v[5] = Vetrex(-1.0f + 2.0f * i, -3.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
				cube[i][j][k].v[6] = Vetrex(-3.0f + 2.0f * i, -3.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);
				cube[i][j][k].v[7] = Vetrex(-3.0f + 2.0f * i, -3.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
				cube[i][j][k].bottom = black;

				//左面
				cube[i][j][k].v[8] = Vetrex(-3.0f + 2.0f * i, -3.0f + 2.0f * j, -1.0f + 2.0f * k, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
				cube[i][j][k].v[9] = Vetrex(-3.0f + 2.0f * i, -1.0f + 2.0f * j, -1.0f + 2.0f * k, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
				cube[i][j][k].v[10] = Vetrex(-3.0f + 2.0f * i, -1.0f + 2.0f * j, -3.0f + 2.0f * k, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
				cube[i][j][k].v[11] = Vetrex(-3.0f + 2.0f * i, -3.0f + 2.0f * j, -3.0f + 2.0f * k, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
				cube[i][j][k].left = black;

				//右面
				cube[i][j][k].v[12] = Vetrex(-1.0f + 2.0f * i, -3.0f + 2.0f * j, -3.0f + 2.0f * k, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
				cube[i][j][k].v[13] = Vetrex(-1.0f + 2.0f * i, -1.0f + 2.0f * j, -3.0f + 2.0f * k, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
				cube[i][j][k].v[14] = Vetrex(-1.0f + 2.0f * i, -1.0f + 2.0f * j, -1.0f + 2.0f * k, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
				cube[i][j][k].v[15] = Vetrex(-1.0f + 2.0f * i, -3.0f + 2.0f * j, -1.0f + 2.0f * k, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);
				cube[i][j][k].right = black;

				//正面
				cube[i][j][k].v[16] = Vetrex(-3.0f + 2.0f * i, -3.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
				cube[i][j][k].v[17] = Vetrex(-3.0f + 2.0f * i, -1.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
				cube[i][j][k].v[18] = Vetrex(-1.0f + 2.0f * i, -1.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
				cube[i][j][k].v[19] = Vetrex(-1.0f + 2.0f * i, -3.0f + 2.0f * j, -3.0f + 2.0f * k, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);
				cube[i][j][k].front = black;

				//背面
				cube[i][j][k].v[20] = Vetrex(-1.0f + 2.0f * i, -3.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
				cube[i][j][k].v[21] = Vetrex(-1.0f + 2.0f * i, -1.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
				cube[i][j][k].v[22] = Vetrex(-3.0f + 2.0f * i, -1.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);
				cube[i][j][k].v[23] = Vetrex(-3.0f + 2.0f * i, -3.0f + 2.0f * j, -1.0f + 2.0f * k, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
				cube[i][j][k].rear = black;

				//一开始是没旋转的状态
				cube[i][j][k].rotating = false;
			}
		}
	}
	//初始化面颜色
	for (int i = 0; i < 3; ++i)
		for (int k = 0; k < 3; ++k)
		{
			cube[i][2][k].top = green;
			cube[i][0][k].bottom = blue;
		}

	for (int j = 0; j < 3; ++j)
		for (int k = 0; k < 3; ++k)
		{
			cube[0][j][k].left = red;
			cube[2][j][k].right = orange;
		}

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			cube[i][j][0].front = white;
			cube[i][j][2].rear = yellow;
		}

	shuffle_times = 30;
	degree = 0;
	start_time = 0;
	key_down_counter = 0;
	XY = YZ = XZ = false;
	complete = false;
	
}

//更新魔方
void Rubik::update()
{
	if (degree > 0) 
		degree -= 6;
	else if (degree < 0) 
		degree += 6;
	else if (!degree && (XY || XZ || YZ))
	{
		XY = XZ = YZ = false;
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				for (int k = 0; k < 3; ++k)
					cube[i][j][k].rotating = false;
	}

	//开局随机打乱
	if (!degree && shuffle_times && !key_down_counter)
	{
		int func = rand() % 12;
		int index = rand() % 3;
		bool inverse = rand() % 2;
		if (func < 3)
			rotate_YZ(index, inverse);
		else if (func < 6)
			rotate_XZ(index, inverse);
		else if (func < 9)
			rotate_XY(index, inverse);
		else if (func < 10)
			view_rotate_X(inverse);
		else if (func < 11)
			view_rotate_Y(inverse);
		else
			view_rotate_Z(inverse);
		shuffle_times--;
		key_down_counter = 18;
	}
}

//显示魔方
void Rubik::show()
{
	D3DXVECTOR4 vec4;
	D3DXVECTOR3 vec3;
	D3DXMATRIX matrix;

	//根据旋转的面来取用对应的旋转矩阵
	if (rubik.XY)
		D3DXMatrixRotationZ(&matrix, D3DX_PI * rubik.degree / 180);
	else if (rubik.YZ)
		D3DXMatrixRotationX(&matrix, D3DX_PI * rubik.degree / 180);
	else if (rubik.XZ)
		D3DXMatrixRotationY(&matrix, D3DX_PI * rubik.degree / 180);

	//顶点缓冲区部分
	int pos = 0;
	Vetrex* vetrices;
	vb->Lock(0, 0, (void**)&vetrices, 0);
	
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
				for (int l = 0; l < 24; ++l)
				{
					//该方块有旋转的话，顶点做旋转处理
					if (rubik.cube[i][j][k].rotating)
					{	
						D3DXVec3Transform(&vec4, &rubik.cube[i][j][k].v[l].pos, &matrix);
						vec3.x = vec4.x; vec3.y = vec4.y; vec3.z = vec4.z;
						vetrices[pos] = rubik.cube[i][j][k].v[l];
						vetrices[pos++].pos = vec3;
					}
					else
						vetrices[pos++] = rubik.cube[i][j][k].v[l];
				}
	vb->Unlock();

	pos = 0;
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

	//显示部分
	d3ddev->SetStreamSource(0, vb, 0, sizeof(Vetrex));
	d3ddev->SetFVF(D3DFVF_MYVETREX);
	d3ddev->SetIndices(ib);
	
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
			{
				d3ddev->SetTexture(0, rubik.cube[i][j][k].top);
				d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (i * 3 * 3 + j * 3 + k) * 24, 4, (i * 3 * 3 + j * 3 + k) * 36, 2);

				d3ddev->SetTexture(0, rubik.cube[i][j][k].bottom);
				d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (i * 3 * 3 + j * 3 + k) * 24 + 4, 4, (i * 3 * 3 + j * 3 + k) * 36 + 6, 2);

				d3ddev->SetTexture(0, rubik.cube[i][j][k].left);
				d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (i * 3 * 3 + j * 3 + k) * 24 + 8, 4, (i * 3 * 3 + j * 3 + k) * 36 + 12, 2);

				d3ddev->SetTexture(0, rubik.cube[i][j][k].right);
				d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (i * 3 * 3 + j * 3 + k) * 24 + 12, 4, (i * 3 * 3 + j * 3 + k) * 36 + 18, 2);

				d3ddev->SetTexture(0, rubik.cube[i][j][k].front);
				d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (i * 3 * 3 + j * 3 + k) * 24 + 16, 4, (i * 3 * 3 + j * 3 + k) * 36 + 24, 2);

				d3ddev->SetTexture(0, rubik.cube[i][j][k].rear);
				d3ddev->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (i * 3 * 3 + j * 3 + k) * 24 + 20, 4, (i * 3 * 3 + j * 3 + k) * 36 + 30, 2);
			}
}

//检测魔方是否完成，如果完成，输出完成用时的窗体
bool Rubik::is_complete(HWND window)
{
	if (complete)
		return true;
	if (degree)		//魔方得停止转动后才能检测
		return false;

	LPDIRECT3DTEXTURE9 color1 = cube[0][2][0].top;
	LPDIRECT3DTEXTURE9 color2 = cube[0][0][0].bottom;
	//顶面底面检测
	for (int i = 0; i < 3; ++i)
		for (int k = 0; k < 3; ++k)
			if (cube[i][2][k].top != color1 || cube[i][0][k].bottom != color2)
				return false;
	
	//左面右面检测
	color1 = cube[0][0][0].left;
	color2 = cube[2][0][0].right;
	for (int j = 0; j < 3; ++j)
		for (int k = 0; k < 3; ++k)
			if (cube[0][j][k].left != color1 || cube[2][j][k].right != color2)
				return false;

	//正面背面检测
	color1 = cube[0][0][0].front;
	color2 = cube[0][0][2].rear;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (cube[i][j][0].front != color1 || cube[i][j][2].rear != color2)
				return false;

	MessageBox(window, (std::string("用时：") + to_string((double)(timeGetTime() - start_time) / 1e3, 3) + "s\n游戏界面按F11可重置").c_str(),
		"恭喜完成", MB_OK);

	complete = true;
	return true;
}

//YZ面旋转，index为0时转动左面，1转动中间，2转动右面
void Rubik::rotate_YZ(int index, bool inverse)
{
	for (int i = inverse ? 7 : 0; inverse ? (i >= 2) : (i < 6); inverse ? --i : ++i)
	{
		std::swap(rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].front,
			rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].bottom);
		std::swap(rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].top,
			rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].front);
		std::swap(rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].rear,
			rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].top);
		std::swap(rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].bottom,
			rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].rear);

		std::swap(rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].left,
			rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].left);
		std::swap(rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].right,
			rubik.cube[index][(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].right);
	}
	
	rubik.degree = inverse ? 90 : -90;
	rubik.YZ = true;
	for (int j = 0; j < 3; ++j)
		for (int k = 0; k < 3; ++k)
			rubik.cube[index][j][k].rotating = true;

}

//XZ面旋转，index为0时转动底面，1转动中间，2转动顶面
void Rubik::rotate_XZ(int index, bool inverse)
{
	for (int i = inverse ? 0 : 7; inverse ? (i < 6) : (i >= 2); inverse ? ++i : --i)
	{
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].front,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].left);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].right,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].front);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].rear,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].right);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].left,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].rear);

		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].top,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].top);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))].bottom,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][index][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))].bottom);
	}

	rubik.degree = inverse ? 90 : -90;
	rubik.XZ = true;
	for (int i = 0; i < 3; ++i)
		for (int k = 0; k < 3; ++k)
			rubik.cube[i][index][k].rotating = true;
}

//YZ面旋转，index为0时转动正面，1转动中间，2转动背面
void Rubik::rotate_XY(int index, bool inverse)
{
	for (int i = inverse ? 0 : 7; inverse ? (i < 6) : (i >= 2); inverse ? ++i : --i)
	{
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))][index].right,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))][index].bottom);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))][index].bottom,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))][index].left);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))][index].left,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))][index].top);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))][index].top,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))][index].right);

		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))][index].front,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))][index].front);
		std::swap(rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * i))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * i))][index].rear,
			rubik.cube[(int)(1 + 1.5 * MySin(D3DX_PI / 4 * (2 + i)))][(int)(1 + 1.5 * MyCos(D3DX_PI / 4 * (2 + i)))][index].rear);
	}

	rubik.degree = inverse ? -90 : 90;
	rubik.XY = true;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			rubik.cube[i][j][index].rotating = true;
}

//绕X轴旋转
void Rubik::view_rotate_X(bool inverse)
{
	for (int i = 0; i < 3; ++i)
		rotate_YZ(i, inverse);
}

//绕Y轴旋转
void Rubik::view_rotate_Y(bool inverse)
{
	for (int i = 0; i < 3; ++i)
		rotate_XZ(i, inverse);
}

//绕Z轴旋转
void Rubik::view_rotate_Z(bool inverse)
{
	for (int i = 0; i < 3; ++i)
		rotate_XY(i, inverse);
}

