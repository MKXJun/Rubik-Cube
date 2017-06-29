#pragma once
#include "DirectX.h"

//魔方的一块小立方体
struct Cube
{
	Vetrex v[24];	//0-3顶面，4-7底面，8-11左面，12-15右面，16-19正面，20-23背面
	LPDIRECT3DTEXTURE9 top, bottom, left, right, front, rear;	//六个面的纹理
	bool rotating;	//该立方体是否在旋转
};

//魔方
struct Rubik
{
	Cube cube[3][3][3];

	POINT position[3][4][4];	//0为顶面 1为左面 2为右面
	int degree;			//旋转的角度
	int shuffle_times;	//初始化时打乱次数
	bool YZ, XZ, XY;	//旋转的面类型
	bool complete;		//魔方是否完成

	void reset();			//重置
	void update();			//更新
	void show();			//显示
	void check_complete(HWND window);		//检测是否完成

	void rotate_YZ(int index, bool inverse);	//YZ面旋转，index为0时转动左面，1转动中间，2转动右面
	void rotate_XZ(int index, bool inverse);	//XZ面旋转，index为0时转动底面，1转动中间，2转动顶面
	void rotate_XY(int index, bool inverse);	//YZ面旋转，index为0时转动正面，1转动中间，2转动背面

	
	void view_rotate_X(bool inverse);			//绕X轴旋转
	void view_rotate_Y(bool inverse);			//绕Y轴旋转
	void view_rotate_Z(bool inverse);			//绕Z轴旋转

	Rubik();	//设置视口顶点坐标

};



