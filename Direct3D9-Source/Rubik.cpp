#include "Rubik.h"


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
	
	while (func_stk.size() > 0)
		func_stk.pop();
}

//更新魔方
void Rubik::update()
{
	//发生鼠标左键点击事件时不要转动魔方
	if (Mouse_Click(0) && (XY || XZ || YZ) && !shuffle_times)
		return;
	else if (degree >= 5)
		degree -= 5;
	else if (degree <= -5)
		degree += 5;
	else if (degree)
		degree = 0;
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
		int func = rand() % 15;
		int index = rand() % 3;
		bool inverse = rand() % 2;
		if (func < 4)
			rotate_YZ(index, inverse);
		else if (func < 8)
			rotate_XZ(index, inverse);
		else if (func < 12)
			rotate_XY(index, inverse);
		else if (func < 13)
			view_rotate_X(inverse);
		else if (func < 14)
			view_rotate_Y(inverse);
		else
			view_rotate_Z(inverse);
		shuffle_times--;
		key_down_counter = 20;
	}
}

//显示魔方
void Rubik::show()
{
	D3DXVECTOR4 vec4;
	D3DXVECTOR3 vec3;
	D3DXMATRIX matrix;

	//显示游戏时间
	std::string str = std::string("用时：");
	if (!start_time)
		str += "0.000s";
	else if (!complete)
		str += std::to_string((timeGetTime() - start_time) / 1000.0f, 3) + "s";
	else
		str += std::to_string((finish_time - start_time) / 1000.0f, 3) + "s";
	font->DrawTextA(nullptr, str.c_str(), str.size(), &font_rect, DT_LEFT, D3DCOLOR_XRGB(255, 255, 255));



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
void Rubik::check_complete(HWND window)
{
	if (complete)
		return;
	if (degree)		//魔方得停止转动后才能检测
		return;

	LPDIRECT3DTEXTURE9 color1 = cube[0][2][0].top;
	LPDIRECT3DTEXTURE9 color2 = cube[0][0][0].bottom;
	//顶面底面检测
	for (int i = 0; i < 3; ++i)
		for (int k = 0; k < 3; ++k)
			if (cube[i][2][k].top != color1 || cube[i][0][k].bottom != color2)
				return;
	
	//左面右面检测
	color1 = cube[0][0][0].left;
	color2 = cube[2][0][0].right;
	for (int j = 0; j < 3; ++j)
		for (int k = 0; k < 3; ++k)
			if (cube[0][j][k].left != color1 || cube[2][j][k].right != color2)
				return;

	//正面背面检测
	color1 = cube[0][0][0].front;
	color2 = cube[0][0][2].rear;
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (cube[i][j][0].front != color1 || cube[i][j][2].rear != color2)
				return;
	//记录完成时间
	finish_time = timeGetTime();
	//保存成绩
	float seconds[6]{ 9999.999f, 9999.999f, 9999.999f, 9999.999f, 9999.999f, 9999.999f };
	std::ifstream fin("user.dat", std::ios::binary);
	if (fin.is_open())
	{
		for (int i = 0; i < 5; ++i)
			fin.read(reinterpret_cast<char*>(&seconds[i]), sizeof(float));
		fin.close();
	}
	seconds[5] = (finish_time - start_time) / 1e3f;
	std::sort(seconds, seconds + 6);
	std::ofstream fout("user.dat", std::ios::binary);
	for (int i = 0; i < 5; ++i)
		fout.write(reinterpret_cast<char*>(&seconds[i]), sizeof(float));
	fout.close();
	MessageBox(window, (std::string("用时：") + std::to_string((float)(finish_time - start_time) / 1e3f, 3) + "s\n游戏界面按F11可重置").c_str(),
		"恭喜完成", MB_OK);
	

	complete = true;
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

//初始化静态描点模式下的魔方顶点在屏幕上的坐标
Rubik::Rubik()
{
	//顶面顶点
	position[0][0][0] = POINT{ 106,206 }; position[0][0][1] = POINT{ 154,232 };
	position[0][0][2] = POINT{ 214,266 }; position[0][0][3] = POINT{ 300,312 };

	position[0][1][0] = POINT{ 188,188 }; position[0][1][1] = POINT{ 234,206 };
	position[0][1][2] = POINT{ 300,232 }; position[0][1][3] = POINT{ 386,266 };

	position[0][2][0] = POINT{ 256,172 }; position[0][2][1] = POINT{ 300,188 };
	position[0][2][2] = POINT{ 366,206 }; position[0][2][3] = POINT{ 446,232 };

	position[0][3][0] = POINT{ 300,160 }; position[0][3][1] = POINT{ 344,172 };
	position[0][3][2] = POINT{ 412,188 }; position[0][3][3] = POINT{ 494,206 };


	//正面顶点
	position[1][0][0] = POINT{ 160,390 }; position[1][0][1] = POINT{ 196,424 };
	position[1][0][2] = POINT{ 242,468 }; position[1][0][3] = POINT{ 300,526 };

	position[1][1][0] = POINT{ 150,348 }; position[1][1][1] = POINT{ 184,376 };
	position[1][1][2] = POINT{ 234,420 }; position[1][1][3] = POINT{ 300,476 };

	position[1][2][0] = POINT{ 130,286 }; position[1][2][1] = POINT{ 168,312 };
	position[1][2][2] = POINT{ 222,352 }; position[1][2][3] = POINT{ 300,406 };

	position[1][3][0] = POINT{ 106,206 }; position[1][3][1] = POINT{ 154,232 };
	position[1][3][2] = POINT{ 214,266 }; position[1][3][3] = POINT{ 300,312 };

	//右面顶点
	position[2][0][0] = POINT{ 300,526 }; position[2][0][1] = POINT{ 358,468 };
	position[2][0][2] = POINT{ 404,424 }; position[2][0][3] = POINT{ 440,390 };

	position[2][1][0] = POINT{ 300,476 }; position[2][1][1] = POINT{ 366,420 };
	position[2][1][2] = POINT{ 416,376 }; position[2][1][3] = POINT{ 450,348 };

	position[2][2][0] = POINT{ 300,406 }; position[2][2][1] = POINT{ 378,352 };
	position[2][2][2] = POINT{ 432,312 }; position[2][2][3] = POINT{ 470,286 };

	position[2][3][0] = POINT{ 300,312 }; position[2][3][1] = POINT{ 386,266 };
	position[2][3][2] = POINT{ 446,232 }; position[2][3][3] = POINT{ 494,206 };
}

