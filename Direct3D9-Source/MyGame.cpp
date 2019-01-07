#include "DirectX.h"
#include "Rubik.h"
extern Rubik rubik;
extern float preview_rotation;
//初始化
bool Game_Init(HWND window)
{
	Direct3D_Init(window);

	//魔方重置即初始化
	rubik.reset();

	//世界变换
	WorldTransform();

	//视角变换
	ViewTransform(pos, target, up);

	//投影变换
	PerspectiveTransform(D3DX_PI * 0.4f, (float)SCREENW / (float)SCREENH, 1.0f, 1000.0f);

	//视口变换使用默认

	return true;
}

void Game_Run(HWND window)
{
	DirectInput_Update();
	
	//控制每秒120帧左右
	if (timeGetTime() - current_time > delay)
	{
		PreView();		//开场视角旋转动画
		rubik.update();	
		Key_Update(window);		//键盘更新
		Mouse_Update(window);	//鼠标更新
		//清屏
		d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(117, 108, 131), 1.0f, 0);

		if (d3ddev->BeginScene())									//开始渲染
		{
			rubik.show();
			

			d3ddev->EndScene();										//停止渲染

			HRESULT hresult = d3ddev->Present(nullptr, nullptr, nullptr, nullptr);	//前台后台交换
			

			if (hresult == D3DERR_DEVICELOST)						//处理设备丢失
				Direct3D_Reset(window);
		}

		frameCnt++;
		
		current_time = timeGetTime();
	}

	//调整帧数在120左右
	if (timeGetTime() - frame_timer > 1000)
	{
		if (frameCnt > 126)
			delay++;
		else if (frameCnt < 114)
			delay--;
		fps = frameCnt;
		frame_timer = timeGetTime();
		frameCnt = 0;
	}
	//检测魔方还原
	if (!rubik.shuffle_times)
		rubik.check_complete(window);

}

void Game_End()
{
	Direct3D_Shutdown();
}

void PreView()
{
	if (pos.x <= 6.0f)
	{
		pos = D3DXVECTOR3(6.0f, 6.0f, -6.0f);
		return;
	}
	pos.x -= 0.01f;
	pos.y -= 0.01f;
	pos.z += 0.01f;
	ViewTransform(pos, target, up);
}

void Key_Update(HWND window)
{
	//按键间隔内禁止操作（魔方在旋转中）
	if (key_down_counter)
	{
		key_down_counter--;
		return;
	}
	//打乱魔方时禁止操作
	if (rubik.shuffle_times)	
		return;
	//鼠标操作时静止键盘操作
	if (slide)	
		return;

	//撤销操作
	if (Key_Down(DIK_LCONTROL) && Key_Down(DIK_Z))
	{
		if (func_stk.size() > 0)
		{
			func_stk.top()();
			func_stk.pop();
		}
		else
			return;
	}
	////拧动魔方部分
	//双面拧动部分
	else if (Key_Down(DIK_LCONTROL) && Key_Down(DIK_I))
	{
		rubik.rotate_YZ(2, false);
		rubik.rotate_YZ(1, false);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 2, true));
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 1, true));
	}
	else if (Key_Down(DIK_LCONTROL) && Key_Down(DIK_K))
	{
		rubik.rotate_YZ(2, true);
		rubik.rotate_YZ(1, true);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 2, false));
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 1, false));
	}
	else if (Key_Down(DIK_LCONTROL) && Key_Down(DIK_J))
	{
		rubik.rotate_XZ(2, false);
		rubik.rotate_XZ(1, false);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 2, true));
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 1, true));
	}
	else if (Key_Down(DIK_LCONTROL) && Key_Down(DIK_L))
	{
		rubik.rotate_XZ(2, true);
		rubik.rotate_XZ(1, true);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 2, false));
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 1, false));
	}
	else if (Key_Down(DIK_LCONTROL) && Key_Down(DIK_U))
	{
		rubik.rotate_XY(0, true);
		rubik.rotate_XY(1, true);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 0, false));
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 1, false));
	}
	else if (Key_Down(DIK_LCONTROL) && Key_Down(DIK_O))
	{
		rubik.rotate_XY(0, false);
		rubik.rotate_XY(1, false);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 0, true));
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 1, true));
	}
	//单面拧动部分
	else if (Key_Down(DIK_I))
	{
		rubik.rotate_YZ(2, false);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 2, true));
	}
	else if (Key_Down(DIK_K))
	{
		rubik.rotate_YZ(2, true);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 2, false));
	}	
	else if (Key_Down(DIK_J))
	{
		rubik.rotate_XZ(2, false);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 2, true));
	}
	else if (Key_Down(DIK_L))
	{
		rubik.rotate_XZ(2, true);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 2, false));
	}
	else if (Key_Down(DIK_U))
	{
		rubik.rotate_XY(0, true);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 0, false));
	}
	else if (Key_Down(DIK_O))
	{
		rubik.rotate_XY(0, false);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 0, true));
	}
		

	else if (Key_Down(DIK_T))
	{
		rubik.rotate_YZ(1, false);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 1, true));
	}
	else if (Key_Down(DIK_G))
	{
		rubik.rotate_YZ(1, true);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 1, false));
	}
	else if (Key_Down(DIK_F))
	{
		rubik.rotate_XZ(1, false);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 1, true));
	}
	else if (Key_Down(DIK_H))
	{
		rubik.rotate_XZ(1, true);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 1, false));
	}
	else if (Key_Down(DIK_R))
	{
		rubik.rotate_XY(1, true);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 1, false));
	}
	else if (Key_Down(DIK_Y))
	{
		rubik.rotate_XY(1, false);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 1, true));
	}


	else if (Key_Down(DIK_W))
	{
		rubik.rotate_YZ(0, false);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 0, true));
	}
	else if (Key_Down(DIK_S))
	{
		rubik.rotate_YZ(0, true);
		func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, 0, false));
	}
	else if (Key_Down(DIK_A))
	{
		rubik.rotate_XZ(0, false);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 0, true));
	}
	else if (Key_Down(DIK_D))
	{
		rubik.rotate_XZ(0, true);
		func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, 0, false));
	}
	else if (Key_Down(DIK_Q))
	{
		rubik.rotate_XY(2, true);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 2, false));
	}
	else if (Key_Down(DIK_E))
	{
		rubik.rotate_XY(2, false);
		func_stk.push(std::bind(&Rubik::rotate_XY, rubik, 2, true));
	}
		

	//整个魔方旋转  部分
	else if (Key_Down(DIK_UP))
	{
		rubik.view_rotate_X(false);
		func_stk.push(std::bind(&Rubik::view_rotate_X, rubik, true));
	}
	else if (Key_Down(DIK_DOWN))
	{
		rubik.view_rotate_X(true);
		func_stk.push(std::bind(&Rubik::view_rotate_X, rubik, false));
	}
	else if (Key_Down(DIK_LEFT))
	{
		rubik.view_rotate_Y(false);
		func_stk.push(std::bind(&Rubik::view_rotate_Y, rubik, true));
	}
	else if (Key_Down(DIK_RIGHT))
	{
		rubik.view_rotate_Y(true);
		func_stk.push(std::bind(&Rubik::view_rotate_Y, rubik, false));
	}
	else if (Key_Down(DIK_PGUP))
	{
		rubik.view_rotate_Z(true);
		func_stk.push(std::bind(&Rubik::view_rotate_Z, rubik, false));
	}
	else if (Key_Down(DIK_PGDN))
	{
		rubik.view_rotate_Z(false);
		func_stk.push(std::bind(&Rubik::view_rotate_Z, rubik, true));
	}
		

	//切换模式判定
	else if (Key_Down(DIK_F9))
	{
		use_pick = !use_pick;
		std::string str = "当前模式为：";
		str += use_pick ? "动态拾取" : "静态描点";
		MessageBox(window, str.c_str(), "注意", MB_OK);
		if (!use_pick)
		{
			pos = D3DXVECTOR3(6.0f, 6.0f, -6.0f);
			ViewTransform(pos, target, up);
		}
		return;
	}
	//复原魔方
	else if (Key_Down(DIK_F10))
	{
		rubik.reset();
		
		rubik.shuffle_times = 0;
		rubik.complete = true;
		return;
	}
	//重置游戏
	else if (Key_Down(DIK_F11))
	{
		rubik.reset();
		pos = D3DXVECTOR3(12.0f, 12.0f, -12.0f);
		return;
	}
	//排行榜
	else if (Key_Down(DIK_F12))
	{
		std::ifstream fin("user.dat", std::ios::binary);
		float second;
		std::string str = "作者：X_Jun\n\n排名    用时\n";
		if (fin.is_open())
		{
			for (int i = 0; i < 5; ++i)
			{
				fin.read(reinterpret_cast<char*>(&second), sizeof(float));
				str += " " + std::to_string(i + 1) + "       " + std::to_string(second, 3) + "s\n";
			}
			fin.close();
		}
		else
		{
			for (int i = 0; i < 5; ++i)
				str += " " + std::to_string(i + 1) + "       9999.999s\n";
		}
		str += "\n";
		MessageBox(window, str.c_str(), "关于魔方", MB_OK);
		return;
	}
	//没检测到按键则提前结束
	else
		return;

	//实质性转动将开始计时
	if (!rubik.complete && !start_time) start_time = timeGetTime();
	key_down_counter = 20;
}

void Mouse_Update(HWND window)
{
	POINT prev_pos = { mouse_pos.x, mouse_pos.y };
	GetCursorPos(&mouse_pos);	//鼠标的全局坐标
	GetWindowRect(window, &window_rect);	//窗口的位置
	//求得鼠标在窗口内的绝对坐标
	mouse_pos.x -= window_rect.left;
	mouse_pos.y -= window_rect.top;

	
	if (rubik.shuffle_times)
		return;
	//增加滑动延迟让拖动效果理想些
	if (slide_delay)
	{
		slide_delay--;
		return;
	}

	//使用动态拾取时，视角可以随着鼠标移动带动魔方轻微旋转
	if (use_pick)
	{
		//水平方向
		D3DXVECTOR3 center{ 6.0f, 6.0f, -6.0f }, raw_up{ -1.0f, 8.0f, 1.0f };
		D3DXVECTOR4 vec4;
		D3DXMATRIX matrix;
		D3DXMatrixRotationY(&matrix, 0.0001f * (mouse_pos.x - 300));
		D3DXVec3Transform(&vec4, &center, &matrix);
		pos.x = vec4.x;
		pos.y = vec4.y;
		pos.z = vec4.z;
		
		D3DXVec3Transform(&vec4, &raw_up, &matrix);
		up.x = vec4.x;
		up.y = vec4.y;
		up.z = vec4.z;

		//竖直方向
		D3DXVECTOR3 axis{ 1.0f, 0.0f, 1.0f };
		D3DXVec3Transform(&vec4, &axis, &matrix);
		axis.x = vec4.x;
		axis.y = vec4.y;
		axis.z = vec4.z;

		D3DXMatrixRotationAxis(&matrix, &axis, 0.0001f * (mouse_pos.y - 300));
		D3DXVec3Transform(&vec4, &pos, &matrix);
		pos.x = vec4.x;
		pos.y = vec4.y;
		pos.z = vec4.z;

		D3DXVec3Transform(&vec4, &up, &matrix);
		up.x = vec4.x;
		up.y = vec4.y;
		up.z = vec4.z;


		ViewTransform(pos, target, up);
	}

	//未触发点击
	if (!Mouse_Click(0))
	{
		if (slide)
		{
			//停止滑动
			slide = false;
			if (rubik.XY || rubik.YZ || rubik.XZ)
			{
				//若滑动产生大幅度转动魔方，进行调整
				int current_degree = rubik.degree;

				//实质性转动将开始计时
				if ((current_degree > 45 || current_degree < -45) && !start_time && !rubik.complete)
					start_time = timeGetTime();

				while (current_degree > 45 || current_degree < -45)
				{
					if (rubik.XY)
					{
						for (int k = 0; k < 3; ++k)
							if (rubik.cube[0][0][k].rotating)
							{
								rubik.rotate_XY(k, current_degree > 45);
								func_stk.push(std::bind(&Rubik::rotate_XY, rubik, k, !(current_degree > 45)));
							}

						if (rubik.cube[0][0][0].rotating && rubik.cube[0][0][1].rotating && rubik.cube[0][0][2].rotating && func_stk.size() >= 3)
						{
							func_stk.pop();
							func_stk.pop();
							func_stk.pop();
							func_stk.push(std::bind(&Rubik::view_rotate_Z, rubik, !(current_degree > 45)));
						}
								
					}
					else if (rubik.YZ)
					{
						for (int i = 0; i < 3; ++i)
							if (rubik.cube[i][0][0].rotating)
							{
								rubik.rotate_YZ(i, current_degree < -45);
								func_stk.push(std::bind(&Rubik::rotate_YZ, rubik, i, !(current_degree < -45)));
							}

						if (rubik.cube[0][0][0].rotating && rubik.cube[1][0][0].rotating && rubik.cube[2][0][0].rotating && func_stk.size() >= 3)
						{
							func_stk.pop();
							func_stk.pop();
							func_stk.pop();
							func_stk.push(std::bind(&Rubik::view_rotate_X, rubik, !(current_degree < -45)));
						}

					}
					else
					{
						for (int j = 0; j < 3; ++j)
							if (rubik.cube[0][j][0].rotating)
							{
								rubik.rotate_XZ(j, current_degree < -45);
								func_stk.push(std::bind(&Rubik::rotate_XZ, rubik, j, !(current_degree < -45)));
							}

						if (rubik.cube[0][0][0].rotating && rubik.cube[0][1][0].rotating && rubik.cube[0][2][0].rotating && func_stk.size() >= 3)
						{
							func_stk.pop();
							func_stk.pop();
							func_stk.pop();
							func_stk.push(std::bind(&Rubik::view_rotate_Y, rubik, !(current_degree < -45)));
						}
								
					}
					current_degree -= current_degree > 45 ? 90 : -90;
					
				}
				rubik.degree = current_degree;
			}
		}
		
		return;
	}
	else if (!slide)
	{
		click_pos = mouse_pos;
		slide = true;
		slide_delay = 5;
		return;
	}
		

	//没有确定转动方向时
	if (!rubik.XY && !rubik.YZ && !rubik.XZ)
	{
		if (click_pos.x == mouse_pos.x && click_pos.y == mouse_pos.y)
			return;
		//判断顶面的点击和滑动方向
		if ((float)(mouse_pos.y - click_pos.y) / (float)(mouse_pos.x - click_pos.x) > 0.0f)
		{
			for (int k = 0; k < 3; ++k)
			{
				if (use_pick ? IsPick(window, rubik.cube[0][2][k].v[0], rubik.cube[0][2][k].v[1], rubik.cube[2][2][k].v[2], rubik.cube[2][2][k].v[3]) :
					Click_Rect(rubik.position[0][k][0], rubik.position[0][k + 1][0], rubik.position[0][k + 1][3], rubik.position[0][k][3]))
				{
					rubik.XY = true;
					for (int i = 0; i < 3; ++i)
						for (int j = 0; j < 3; ++j)
							rubik.cube[i][j][k].rotating = true;
					return;
				}
			}
		}
		else
		{
			for (int i = 0; i < 3; ++i)
			{
				if (use_pick ? IsPick(window, rubik.cube[i][2][0].v[0], rubik.cube[i][2][2].v[1], rubik.cube[i][2][2].v[2], rubik.cube[i][2][0].v[3]) :
					Click_Rect(rubik.position[0][0][i], rubik.position[0][3][i], rubik.position[0][3][i + 1], rubik.position[0][0][i + 1]))
				{
					rubik.YZ = true;
					for (int j = 0; j < 3; ++j)
						for (int k = 0; k < 3; ++k)
							rubik.cube[i][j][k].rotating = true;
					return;
				}
			}
		}

		//判断正面的点击和滑动方向
		if (fabs((float)(mouse_pos.y - click_pos.y) / (float)(mouse_pos.x - click_pos.x)) < 1.0f)
		{
			for (int j = 0; j < 3; ++j)
			{
				if (use_pick ? IsPick(window, rubik.cube[0][j][0].v[16], rubik.cube[0][j][0].v[17], rubik.cube[2][j][0].v[18], rubik.cube[2][j][0].v[19]) :
					Click_Rect(rubik.position[1][j][0], rubik.position[1][j + 1][0], rubik.position[1][j + 1][3], rubik.position[1][j][3]))
				{
					rubik.XZ = true;
					for (int i = 0; i < 3; ++i)
						for (int k = 0; k < 3; ++k)
							rubik.cube[i][j][k].rotating = true;
					return;
				}
			}
		}
		else
		{
			for (int i = 0; i < 3; ++i)
			{
				if (use_pick ? IsPick(window, rubik.cube[i][0][0].v[16], rubik.cube[i][2][0].v[17], rubik.cube[i][2][0].v[18], rubik.cube[i][0][0].v[19]) :
					Click_Rect(rubik.position[1][0][i], rubik.position[1][3][i], rubik.position[1][3][i + 1], rubik.position[1][0][i + 1]))
				{
					rubik.YZ = true;
					for (int j = 0; j < 3; ++j)
						for (int k = 0; k < 3; ++k)
							rubik.cube[i][j][k].rotating = true;
					return;
				}
			}
		}
		
		//判断右面的点击和滑动方向
		if (((mouse_pos.y - click_pos.y) && !(mouse_pos.x - click_pos.x)) || fabs((float)(mouse_pos.y - click_pos.y) / (float)(mouse_pos.x - click_pos.x)) > 1.0f)
		{
			for (int k = 0; k < 3; ++k)
			{
				if (use_pick ? IsPick(window, rubik.cube[2][0][k].v[12], rubik.cube[2][2][k].v[13], rubik.cube[2][2][k].v[14], rubik.cube[2][0][k].v[15]) :
					Click_Rect(rubik.position[2][0][k], rubik.position[2][3][k], rubik.position[2][3][k + 1], rubik.position[2][0][k + 1]))
				{
					rubik.XY = true;
					for (int i = 0; i < 3; ++i)
						for (int j = 0; j < 3; ++j)
							rubik.cube[i][j][k].rotating = true;
					return;
				}
			}
		}
		else
		{
			for (int j = 0; j < 3; ++j)
			{
				if (use_pick ? IsPick(window,rubik.cube[2][j][0].v[12], rubik.cube[2][j][0].v[13], rubik.cube[2][j][2].v[14], rubik.cube[2][j][2].v[15]) :
					Click_Rect(rubik.position[2][j][0], rubik.position[2][j + 1][0], rubik.position[2][j + 1][3], rubik.position[2][j][3]))
				{
					rubik.XZ = true;
					for (int i = 0; i < 3; ++i)
						for (int k = 0; k < 3; ++k)
							rubik.cube[i][j][k].rotating = true;
					return;
				}
			}
		}

		//若在魔方外拖动，旋转整个魔方
		if (Click_Rect(POINT{ 0,599 }, POINT{ 0,0 }, POINT{ 299,0 }, POINT{ 299,599 }))
		{
			if (fabs((float)(mouse_pos.y - click_pos.y) / (float)(mouse_pos.x - click_pos.x)) < 1.0f)
				rubik.XZ = true;
			else
				rubik.YZ = true;
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					for (int k = 0; k < 3; ++k)
						rubik.cube[i][j][k].rotating = true;
		}
		else if (Click_Rect(POINT{ 300, 599 }, POINT{ 300, 0 }, POINT{ 599, 0 }, POINT{ 599, 599 }))
		{
			if (fabs((float)(mouse_pos.y - click_pos.y) / (float)(mouse_pos.x - click_pos.x)) < 1.0f)
				rubik.XZ = true;
			else
				rubik.XY = true;
			for (int i = 0; i < 3; ++i)
				for (int j = 0; j < 3; ++j)
					for (int k = 0; k < 3; ++k)
						rubik.cube[i][j][k].rotating = true;
		}
	}
	//此时已经确定拖动方向，鼠标拖动产生旋转
	else
	{
		if (rubik.YZ)
		{
			rubik.degree += (int)(-(mouse_pos.y - prev_pos.y) / 2.0f);
			rubik.degree += (int)((mouse_pos.x - prev_pos.x) / 3.0f);
		}
		else if (rubik.XZ)
		{
			rubik.degree += (int)(-(mouse_pos.x - prev_pos.x) / 2.0f);
		}
		else if (rubik.XY)
		{
			rubik.degree += (int)(-(mouse_pos.y - prev_pos.y) / 2.0f);
			rubik.degree += (int)(-(mouse_pos.x - prev_pos.x) / 3.0f);
		}
	}
}

//点击区域，适用于任何凸四边形，需要按顺时针提供
bool Click_Rect(const POINT& LeftBottom, const POINT& LeftTop, const POINT& RightTop, const POINT& RightBottom)
{
	D3DXVECTOR3 vec1, vec2, cross;
	
	vec1 = D3DXVECTOR3((float)(mouse_pos.x - LeftBottom.x), (float)(mouse_pos.y - LeftBottom.y), 0.0f);
	vec2 = D3DXVECTOR3((float)(RightBottom.x - LeftBottom.x), (float)(RightBottom.y - LeftBottom.y), 0.0f);
	D3DXVec3Cross(&cross, &vec1, &vec2);
	if (cross.z < 0.0f)
		return false;

	vec1 = D3DXVECTOR3((float)(mouse_pos.x - LeftTop.x), (float)(mouse_pos.y - LeftTop.y), 0.0f);
	vec2 = D3DXVECTOR3((float)(LeftBottom.x - LeftTop.x), (float)(LeftBottom.y - LeftTop.y), 0.0f);
	D3DXVec3Cross(&cross, &vec1, &vec2);
	if (cross.z < 0.0f)
		return false;

	vec1 = D3DXVECTOR3((float)(mouse_pos.x - RightTop.x), (float)(mouse_pos.y - RightTop.y), 0.0f);
	vec2 = D3DXVECTOR3((float)(LeftTop.x - RightTop.x), (float)(LeftTop.y - RightTop.y), 0.0f);
	D3DXVec3Cross(&cross, &vec1, &vec2);
	if (cross.z < 0.0f)
		return false;

	vec1 = D3DXVECTOR3((float)(mouse_pos.x - RightBottom.x), (float)(mouse_pos.y - RightBottom.y), 0.0f);
	vec2 = D3DXVECTOR3((float)(RightTop.x - RightBottom.x), (float)(RightTop.y - RightBottom.y), 0.0f);
	D3DXVec3Cross(&cross, &vec1, &vec2);
	if (cross.z < 0.0f)
		return false;


	return true;
}

//鼠标拾取
bool IsPick(HWND window, const Vetrex& LeftBottom, const Vetrex& LeftTop, const Vetrex& RightTop, const Vetrex& RightBottom)
{
	//获取世界矩阵、取景变换矩阵、投影矩阵、视口
	D3DXMATRIX matrixWorld, matrixView, matrixProj;
	D3DVIEWPORT9 viewPort;
	d3ddev->GetTransform(D3DTS_WORLD, &matrixWorld);
	d3ddev->GetTransform(D3DTS_VIEW, &matrixView);
	d3ddev->GetTransform(D3DTS_PROJECTION, &matrixProj);
	d3ddev->GetViewport(&viewPort);


	/*	视口到投影变换，获取投影到鼠标位置的向量。
		实际的视口是指窗口内部不含边框的部分，因此实际宽度为584，高度为554，而不是默认的600 * 600
		窗口的上边框高度为38，下左右边框宽度均为8
		如果做成无边框，则需要删去被减掉的边框宽度值
	*/
	float px = (float)(((2.0f * (mouse_pos.x - 8)) / (viewPort.Width - 16)) - 1.0f) / matrixProj(0, 0);
	float py = (float)(((-2.0f * (mouse_pos.y - 38)) / (viewPort.Height - 46)) + 1.0f) / matrixProj(1, 1);
	D3DXVECTOR3 direction(px, py, 1.0f);
	D3DXVECTOR3 start_pos(0.0f, 0.0f, 0.0f);	//必须设为全0.0f
	
	D3DXMatrixInverse(&matrixView, 0, &matrixView);
	//获得摄影机坐标
	D3DXVec3TransformCoord(&start_pos, &start_pos, &matrixView);
	//标准化法向量
	D3DXVec3TransformNormal(&direction, &direction, &matrixView);
	D3DXVec3Normalize(&direction, &direction);

	float u, v, dist;
	bool r1 = D3DXIntersectTri(&LeftBottom.pos, &LeftTop.pos, &RightTop.pos, &start_pos, &direction, &u, &v, &dist);
	bool r2 = D3DXIntersectTri(&RightTop.pos, &RightBottom.pos, &LeftBottom.pos, &start_pos, &direction, &u, &v, &dist);
	return r1 || r2;
}

//带精度浮点数转换
_STD_BEGIN
std::string to_string(float _Val, int precision)
{
	std::string str;
	str += "%." + std::to_string(precision) + "f";
	return (std::_Floating_to_string(str.c_str(), _Val));
	//使用微软VS内置函数
}
_STD_END

