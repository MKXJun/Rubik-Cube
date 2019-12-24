#include "Rubik.h"
#include "d3dUtil.h"
#include "Vertex.h"
#include <fstream>
using namespace DirectX;
using namespace Microsoft::WRL;

DirectX::XMMATRIX Cube::GetWorldMatrix() const
{
	XMVECTOR posVec = XMLoadFloat3(&pos);
	// rotation必然最多只有一个分量是非0，保证其只会绕其中一个轴进行旋转
	XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	posVec = XMVector3TransformCoord(posVec, R);
	// 立方体转动后最终的位置
	XMFLOAT3 finalPos;
	XMStoreFloat3(&finalPos, posVec);

	return XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		XMMatrixTranslation(finalPos.x, finalPos.y, finalPos.z);
}

Rubik::Rubik()
	: mRotationSpeed(XM_2PI)
{
}

void Rubik::InitResources(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{

	std::vector<std::wstring> filenames {
		L"Resource/Black.dds",
		L"Resource/Orange.dds",
		L"Resource/Red.dds",
		L"Resource/Green.dds",
		L"Resource/Blue.dds",
		L"Resource/Yellow.dds",
		L"Resource/White.dds",
	};

	// 检验所有文件是否存在
	bool fileExists = true;
	for (const std::wstring& filename : filenames)
	{
		std::wifstream wfin(filename);
		if (!wfin.is_open())
		{
			fileExists = false;
			wfin.close();
			break;
		}
		wfin.close();
	}
	if (fileExists)
	{
		// 从文件读取
		mTexArray = CreateDDSTexture2DArrayFromFile(device, deviceContext, filenames);
	}
	else
	{
		// 从内存读取
		// 后续可能会写专门的通用函数
		mTexArray = CreateRubikCubeTextureArrayFromMemory(device, deviceContext);
	}

	//
	// 初始化立方体网格模型
	//

	VertexPosTex vertices[] = {
		// +X面
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		// -X面
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		// +Y面
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		// -Y面
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		// +Z面
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		// -Z面
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	// 设置顶点缓冲区描述
	D3D11_BUFFER_DESC vbd;
	ZeroMemory(&vbd, sizeof(vbd));
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof vertices;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	// 新建顶点缓冲区
	D3D11_SUBRESOURCE_DATA initData;
	ZeroMemory(&initData, sizeof(initData));
	initData.pSysMem = vertices;
	HR(device->CreateBuffer(&vbd, &initData, mVertexBuffer.ReleaseAndGetAddressOf()));
	

	WORD indices[] = { 0, 1, 2, 2, 3, 0 };
	// 设置索引缓冲区描述
	D3D11_BUFFER_DESC ibd;
	ZeroMemory(&ibd, sizeof(ibd));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof indices;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	// 新建索引缓冲区
	initData.pSysMem = indices;
	HR(device->CreateBuffer(&ibd, &initData, mIndexBuffer.ReleaseAndGetAddressOf()));

	// 初始化魔方所有面
	Reset();

	// 预先绑定顶点/索引缓冲区到渲染管线
	UINT strides[1] = { sizeof(VertexPosTex) };
	UINT offsets[1] = { 0 };
	deviceContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), strides, offsets);
	deviceContext->IASetIndexBuffer(mIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

}

void Rubik::Reset()
{
	mIsLocked = false;
	mIsPressed = false;

	// 初始化魔方中心位置，用六个面默认填充黑色
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
			{
				mCubes[i][j][k].pos = XMFLOAT3(-2.0f + 2.0f * i,
					-2.0f + 2.0f * j, -2.0f + 2.0f * k);
				mCubes[i][j][k].rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
				memset(mCubes[i][j][k].faceColors, 0, 
					sizeof mCubes[i][j][k].faceColors);
			}
	
	// +X面为橙色，-X面为红色
	// +Y面为绿色，-Y面为蓝色
	// +Z面为黄色，-Z面为白色
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			mCubes[2][i][j].faceColors[RubikFace_PosX] = RubikFaceColor_Orange;
			mCubes[0][i][j].faceColors[RubikFace_NegX] = RubikFaceColor_Red;

			mCubes[j][2][i].faceColors[RubikFace_PosY] = RubikFaceColor_Green;
			mCubes[j][0][i].faceColors[RubikFace_NegY] = RubikFaceColor_Blue;

			mCubes[i][j][2].faceColors[RubikFace_PosZ] = RubikFaceColor_Yellow;
			mCubes[i][j][0].faceColors[RubikFace_NegZ] = RubikFaceColor_White;
		}	


}

void Rubik::Update(float dt)
{
	if (mIsLocked)
	{
		int finishCount = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				for (int k = 0; k < 3; ++k)
				{
					// 令x，y, z轴向旋转角度逐渐归0
					// x轴
					float dTheta = (signbit(mCubes[i][j][k].rotation.x) ? -1.0f : 1.0f) * dt * mRotationSpeed;
					if (fabs(mCubes[i][j][k].rotation.x) < fabs(dTheta))
					{
						mCubes[i][j][k].rotation.x = 0.0f;
						finishCount++;
					}
					else
					{
						mCubes[i][j][k].rotation.x -= dTheta;
					}
					// y轴
					dTheta = (signbit(mCubes[i][j][k].rotation.y) ? -1.0f : 1.0f) * dt * mRotationSpeed;
					if (fabs(mCubes[i][j][k].rotation.y) < fabs(dTheta))
					{
						mCubes[i][j][k].rotation.y = 0.0f;
						finishCount++;
					}
					else
					{
						mCubes[i][j][k].rotation.y -= dTheta;
					}
					// z轴
					dTheta = (signbit(mCubes[i][j][k].rotation.z) ? -1.0f : 1.0f) * dt * mRotationSpeed;
					if (fabs(mCubes[i][j][k].rotation.z) < fabs(dTheta))
					{
						mCubes[i][j][k].rotation.z = 0.0f;
						finishCount++;
					}
					else
					{
						mCubes[i][j][k].rotation.z -= dTheta;
					}
				}
			}
		}

		// 所有方块都结束动画才能解锁
		if (finishCount == 81)
			mIsLocked = false;
	}
}

void Rubik::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect)
{
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				effect.SetWorldMatrix(mCubes[i][j][k].GetWorldMatrix());
				for (int face = 0; face < 6; ++face)
				{
					effect.SetTexIndex(mCubes[i][j][k].faceColors[face]);
					effect.Apply(deviceContext);
					deviceContext->DrawIndexed(6, 0, 4 * face);
				}
			}
		}
	}	
}

bool Rubik::IsLocked() const
{
	return mIsLocked;
}

bool Rubik::IsCompleted() const
{
	RubikFaceColor posX, negX, posY, negY, posZ, negZ;
	posX = mCubes[2][0][0].faceColors[0];
	negX = mCubes[0][0][0].faceColors[1];
	posY = mCubes[0][2][0].faceColors[2];
	negY = mCubes[0][0][0].faceColors[3];
	posZ = mCubes[0][0][2].faceColors[4];
	negZ = mCubes[0][0][0].faceColors[5];

	for (int j = 0; j < 3; ++j)
		for (int k = 0; k < 3; ++k)
			if (mCubes[2][j][k].faceColors[0] != posX || mCubes[0][j][k].faceColors[1] != negX)
				return false;

	for (int k = 0; k < 3; ++k)
		for (int i = 0; i < 3; ++i)
			if (mCubes[i][2][k].faceColors[2] != posY || mCubes[i][0][k].faceColors[3] != negY)
				return false;

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			if (mCubes[i][j][2].faceColors[4] != posZ || mCubes[i][j][0].faceColors[5] != negZ)
				return false;

	return true;
}

DirectX::XMINT3 Rubik::HitCube(Ray ray, float * pDist) const
{
	BoundingOrientedBox box(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f));
	BoundingOrientedBox transformedBox;
	XMINT3 res = XMINT3(-1, -1, -1);
	float dist, minDist = FLT_MAX;

	// 优先拾取暴露在外的立方体(同时也是距离摄像机最近的)
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				box.Transform(transformedBox, mCubes[i][j][k].GetWorldMatrix());
				if (ray.Hit(transformedBox, &dist) && dist < minDist)
				{
					minDist = dist;
					res = XMINT3(i, j, k);
				}
			}
		}
	}
	if (pDist)
		*pDist = (minDist == FLT_MAX ? 0.0f : minDist);
		
	return res;
}

void Rubik::RotateX(int pos, float dTheta, bool isPressed)
{
	if (!mIsLocked)
	{
		// 检验当前是否为键盘操作
		// 可以认为仅当键盘操作时才会产生绝对值为pi/2的倍数(不包括0)的瞬时值
		bool isKeyOp =  static_cast<int>(round(dTheta / XM_PIDIV2)) != 0 &&
			(fabs(fmod(dTheta, XM_PIDIV2) < 1e-5f));
		// 键盘输入和鼠标操作互斥，拒绝键盘的操作
		if (mIsPressed && isKeyOp)
		{
			return;
		}

		mIsPressed = isPressed;

		// 更新旋转状态
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
			{
				switch (pos)
				{
				case 3: mCubes[0][j][k].rotation.x += dTheta;
				case -2: mCubes[1][j][k].rotation.x += dTheta;
					mCubes[2][j][k].rotation.x += dTheta;
					break;
				case -1: mCubes[0][j][k].rotation.x += dTheta; 
					mCubes[1][j][k].rotation.x += dTheta; 
					break;
				
				default: mCubes[pos][j][k].rotation.x += dTheta;
				}
				
			}
				

		// 鼠标或键盘操作完成
		if (!mIsPressed)
		{
			// 开始动画演示状态
			mIsLocked = true;

			// 进行预旋转
			PreRotateX(isKeyOp);
		}
	}
}

void Rubik::RotateY(int pos, float dTheta, bool isPressed)
{
	if (!mIsLocked)
	{
		// 检验当前是否为键盘操作
		// 可以认为仅当键盘操作时才会产生绝对值为pi/2的倍数(不包括0)的瞬时值
		bool isKeyOp = static_cast<int>(round(dTheta / XM_PIDIV2)) != 0 &&
			(fabs(fmod(dTheta, XM_PIDIV2) < 1e-5f));
		// 键盘输入和鼠标操作互斥，拒绝键盘的操作
		if (mIsPressed && isKeyOp)
		{
			return;
		}

		

		for (int k = 0; k < 3; ++k)
			for (int i = 0; i < 3; ++i)
			{
				switch (pos)
				{
				case 3: mCubes[i][0][k].rotation.y += dTheta;
				case -2: mCubes[i][1][k].rotation.y += dTheta;
					mCubes[i][2][k].rotation.y += dTheta;
					break;
				case -1: mCubes[i][0][k].rotation.y += dTheta;
					mCubes[i][1][k].rotation.y += dTheta;
					break;
				
				default: mCubes[i][pos][k].rotation.y += dTheta;
				}
			}

		mIsPressed = isPressed;

		// 鼠标或键盘操作完成
		if (!mIsPressed)
		{
			// 开始动画演示状态
			mIsLocked = true;

			// 进行预旋转
			PreRotateY(isKeyOp);
		}
	}
}

void Rubik::RotateZ(int pos, float dTheta, bool isPressed)
{
	if (!mIsLocked)
	{
		// 检验当前是否为键盘操作
		// 可以认为仅当键盘操作时才会产生绝对值为pi/2的倍数(不包括0)的瞬时值
		bool isKeyOp = static_cast<int>(round(dTheta / XM_PIDIV2)) != 0 &&
			(fabs(fmod(dTheta, XM_PIDIV2) < 1e-5f));
		// 键盘输入和鼠标操作互斥，拒绝键盘的操作
		if (mIsPressed && isKeyOp)
		{
			return;
		}

		mIsPressed = isPressed;

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
			{
				switch (pos)
				{
				case 3: mCubes[i][j][0].rotation.z += dTheta;
				case -2: mCubes[i][j][1].rotation.z += dTheta;
					mCubes[i][j][2].rotation.z += dTheta;
					break;
				case -1: mCubes[i][j][0].rotation.z += dTheta;
					mCubes[i][j][1].rotation.z += dTheta;
					break;
				
				default: mCubes[i][j][pos].rotation.z += dTheta;
				}
			}

				

		// 鼠标或键盘操作完成
		if (!mIsPressed)
		{
			// 开始动画演示状态
			mIsLocked = true;

			// 进行预旋转
			PreRotateZ(isKeyOp);
		}
	}
}

void Rubik::SetRotationSpeed(float rad)
{
	assert(rad > 0.0f);
	mRotationSpeed = rad;
}

ComPtr<ID3D11ShaderResourceView> Rubik::GetTexArray() const
{
	return mTexArray;
}

ComPtr<ID3D11ShaderResourceView> Rubik::CreateRubikCubeTextureArrayFromMemory(
	ComPtr<ID3D11Device> device,
	ComPtr<ID3D11DeviceContext> deviceContext)
{
	// 只有文件缺失的情况才会来到这里
	// 从内存创建

	// 创建纹理数组
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = 128;
	texArrayDesc.Height = 128;
	texArrayDesc.MipLevels = 0;	// 指定后将生成完整mipmap链
	texArrayDesc.ArraySize = 7;
	texArrayDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texArrayDesc.SampleDesc.Count = 1;		// 不使用多重采样
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;	// 生成mipmap需要绑定渲染目标
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;	// 指定需要生成mipmap

	ComPtr<ID3D11Texture2D> texArray;
	HR(device->CreateTexture2D(&texArrayDesc, nullptr, texArray.GetAddressOf()));
	// 创建后立马获取纹理数组描述以获取生成的mipLevel
	texArray->GetDesc(&texArrayDesc);

	// (r, g, b, a)
	unsigned colors[7] = {
		'\x0\x0\x0\xff',		// 黑色
		'\xff\x6c\x0\xff',		// 橙色
		'\xdc\x42\x2f\xff',		// 红色
		'\x0\x9d\x54\xff',		// 绿色
		'\x3d\x81\xf6\xff',		// 蓝色
		'\xfd\xcc\x9\xff',		// 黄色
		'\xff\xff\xff\xff'		// 白色
	};


	uint32_t textureMap[128][128];
	// 默认先创建黑色
	for (int i = 0; i < 128; ++i)
		for (int j = 0; j < 128; ++j)
			textureMap[i][j] = colors[0];

	deviceContext->UpdateSubresource(texArray.Get(),
		D3D11CalcSubresource(0, 0, texArrayDesc.MipLevels),
		nullptr,
		textureMap, 
		128 * 4,
		128 * 128 * 4
	);
	// 创建其它颜色的纹理
	for (int i = 1; i <= 6; ++i)
	{
		for (int y = 7; y <= 17; ++y)
			for (int x = 25 - y; x <= 102 + y; ++x)
				textureMap[y][x] = textureMap[127 - y][x] = colors[i];

		for (int y = 18; y <= 109; ++y)
			for (int x = 7; x <= 120; ++x)
				textureMap[y][x] = colors[i];


		// 更新数据
		deviceContext->UpdateSubresource(texArray.Get(),
			D3D11CalcSubresource(0, i, texArrayDesc.MipLevels),
			nullptr,
			textureMap,
			128 * 4,
			128 * 128 * 4
		);

	}
	// 创建纹理数组的SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = -1;	// 生成mipamp
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = 7;

	ComPtr<ID3D11ShaderResourceView> texArraySRV;
	HR(device->CreateShaderResourceView(texArray.Get(), &viewDesc, texArraySRV.GetAddressOf()));
	// 生成mipmap
	deviceContext->GenerateMips(texArraySRV.Get());
	return texArraySRV;
}

void Rubik::PreRotateX(bool isKeyOp)
{
	for (int i = 0; i < 3; ++i)
	{
		// 当前层没有旋转则直接跳过
		if (fabs(mCubes[i][0][0].rotation.x) < 1e-5f)
			continue;
		// 由于此时被旋转面的所有方块旋转角度都是一样的，可以从中取一个来计算。
		// 计算归位回[-pi/4, pi/4)区间需要顺时针旋转90度的次数
		int times = static_cast<int>(round(mCubes[i][0][0].rotation.x / XM_PIDIV2));
		// 将归位次数映射到[0, 3]，以计算最小所需顺时针旋转90度的次数
		int minTimes = (times % 4 + 4) % 4;

		// 调整所有被旋转方块的初始角度
		for (int j = 0; j < 3; ++j)
		{
			for (int k = 0; k < 3; ++k)
			{
				// 键盘按下后的变化
				if (isKeyOp)
				{
					// 顺时针旋转90度--->实际演算从-90度加到0度
					// 逆时针旋转90度--->实际演算从90度减到0度
					mCubes[i][j][k].rotation.x *= -1.0f;
				}
				// 鼠标释放后的变化
				else
				{
					// 归位回[-pi/4, pi/4)的区间
					mCubes[i][j][k].rotation.x -= times * XM_PIDIV2;
				}
			}
		}

		std::vector<XMINT2> indices1, indices2;
		GetSwapIndexArray(minTimes, indices1, indices2);
		size_t swapTimes = indices1.size();
		for (size_t idx = 0; idx < swapTimes; ++idx)
		{
			// 对这两个立方体按规则进行面的交换
			XMINT2 srcIndex = indices1[idx];
			XMINT2 targetIndex = indices2[idx];
			// 若为2次顺时针旋转，则只需4次对角调换
			// 否则，需要6次邻角(棱)对换
			for (int face = 0; face < 6; ++face)
			{
				std::swap(mCubes[i][srcIndex.x][srcIndex.y].faceColors[face],
					mCubes[i][targetIndex.x][targetIndex.y].faceColors[
						GetTargetSwapFaceRotationX(static_cast<RubikFace>(face), minTimes)]);
			}
		}
	}
}

void Rubik::PreRotateY(bool isKeyOp)
{
	for (int j = 0; j < 3; ++j)
	{
		// 当前层没有旋转则直接跳过
		if (fabs(mCubes[0][j][0].rotation.y) < 1e-5f)
			continue;
		// 由于此时被旋转面的所有方块旋转角度都是一样的，可以从中取一个来计算。
		// 计算归位回[-pi/4, pi/4)区间需要顺时针旋转90度的次数
		int times = static_cast<int>(round(mCubes[0][j][0].rotation.y / XM_PIDIV2));
		// 将归位次数映射到[0, 3]，以计算最小所需顺时针旋转90度的次数
		int minTimes = (times % 4 + 4) % 4;

		// 调整所有被旋转方块的初始角度
		for (int k = 0; k < 3; ++k)
		{
			for (int i = 0; i < 3; ++i)
			{
				// 可以认为仅当键盘操作时才会产生绝对值为pi/2的瞬时值
				// 键盘按下后的变化
				if (isKeyOp)
				{
					// 顺时针旋转90度--->实际演算从-90度加到0度
					// 逆时针旋转90度--->实际演算从90度减到0度
					mCubes[i][j][k].rotation.y *= -1.0f;
				}
				// 鼠标释放后的变化
				else
				{
					// 归位回[-pi/4, pi/4)的区间
					mCubes[i][j][k].rotation.y -= times * XM_PIDIV2;
				}
			}
		}

		std::vector<XMINT2> indices1, indices2;
		GetSwapIndexArray(minTimes, indices1, indices2);
		size_t swapTimes = indices1.size();
		for (size_t idx = 0; idx < swapTimes; ++idx)
		{
			// 对这两个立方体按规则进行面的交换
			XMINT2 srcIndex = indices1[idx];
			XMINT2 targetIndex = indices2[idx];
			// 若为2次顺时针旋转，则只需4次对角调换
			// 否则，需要6次邻角(棱)对换
			for (int face = 0; face < 6; ++face)
			{
				std::swap(mCubes[srcIndex.y][j][srcIndex.x].faceColors[face],
					mCubes[targetIndex.y][j][targetIndex.x].faceColors[
						GetTargetSwapFaceRotationY(static_cast<RubikFace>(face), minTimes)]);
			}
		}
	}
}

void Rubik::PreRotateZ(bool isKeyOp)
{
	for (int k = 0; k < 3; ++k)
	{
		// 当前层没有旋转则直接跳过
		if (fabs(mCubes[0][0][k].rotation.z) < 1e-5f)
			continue;

		// 由于此时被旋转面的所有方块旋转角度都是一样的，可以从中取一个来计算。
		// 计算归位回[-pi/4, pi/4)区间需要顺时针旋转90度的次数
		int times = static_cast<int>(round(mCubes[0][0][k].rotation.z / XM_PIDIV2));
		// 将归位次数映射到[0, 3]，以计算最小所需顺时针旋转90度的次数
		int minTimes = (times % 4 + 4) % 4;

		// 调整所有被旋转方块的初始角度
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				// 可以认为仅当键盘操作时才会产生绝对值为pi/2的瞬时值
				// 键盘按下后的变化
				if (isKeyOp)
				{
					// 顺时针旋转90度--->实际演算从-90度加到0度
					// 逆时针旋转90度--->实际演算从90度减到0度
					mCubes[i][j][k].rotation.z *= -1.0f;
				}
				// 鼠标释放后的变化
				else
				{
					// 归位回[-pi/4, pi/4)的区间
					mCubes[i][j][k].rotation.z -= times * XM_PIDIV2;
				}
			}
		}

		std::vector<XMINT2> indices1, indices2;
		GetSwapIndexArray(minTimes, indices1, indices2);
		size_t swapTimes = indices1.size();
		for (size_t idx = 0; idx < swapTimes; ++idx)
		{
			// 对这两个立方体按规则进行面的交换
			XMINT2 srcIndex = indices1[idx];
			XMINT2 targetIndex = indices2[idx];
			// 若为2次顺时针旋转，则只需4次对角调换
			// 否则，需要6次邻角(棱)对换
			for (int face = 0; face < 6; ++face)
			{
				std::swap(mCubes[srcIndex.x][srcIndex.y][k].faceColors[face],
					mCubes[targetIndex.x][targetIndex.y][k].faceColors[
						GetTargetSwapFaceRotationZ(static_cast<RubikFace>(face), minTimes)]);
			}
		}
	}
}

void Rubik::GetSwapIndexArray(int minTimes, std::vector<DirectX::XMINT2>& outArr1, std::vector<DirectX::XMINT2>& outArr2) const
{
	// 进行一次顺时针90度旋转相当逆时针交换6次(顶角和棱各3次)
	// 1   2   4   2   4   2   4   1
	//   *   ->  *   ->  *   ->  *
	// 4   3   1   3   3   1   3   2
	if (minTimes == 1)
	{
		outArr1 = { XMINT2(0, 0), XMINT2(0, 1), XMINT2(0, 2), XMINT2(1, 2), XMINT2(2, 2), XMINT2(2, 1) };
		outArr2 = { XMINT2(0, 2), XMINT2(1, 2), XMINT2(2, 2), XMINT2(2, 1), XMINT2(2, 0), XMINT2(1, 0) };
	}
	// 进行一次顺时针90度旋转相当逆时针交换4次(顶角和棱各2次)
	// 1   2   3   2   3   4
	//   *   ->  *   ->  *  
	// 4   3   4   1   2   1
	else if (minTimes == 2)
	{
		outArr1 = { XMINT2(0, 0), XMINT2(0, 1), XMINT2(0, 2), XMINT2(1, 2) };
		outArr2 = { XMINT2(2, 2), XMINT2(2, 1), XMINT2(2, 0), XMINT2(1, 0) };
	}
	// 进行一次顺时针90度旋转相当逆时针交换6次(顶角和棱各3次)
	// 1   2   4   2   4   2   4   1
	//   *   ->  *   ->  *   ->  *
	// 4   3   1   3   3   1   3   2
	else if (minTimes == 3)
	{
		outArr1 = { XMINT2(0, 0), XMINT2(1, 0), XMINT2(2, 0), XMINT2(2, 1), XMINT2(2, 2), XMINT2(1, 2) };
		outArr2 = { XMINT2(2, 0), XMINT2(2, 1), XMINT2(2, 2), XMINT2(1, 2), XMINT2(0, 2), XMINT2(0, 1) };
	}
	// 0次顺时针旋转不变，其余异常数值也不变
	else
	{
		outArr1.clear();
		outArr2.clear();
	}
	
}

RubikFace Rubik::GetTargetSwapFaceRotationX(RubikFace face, int times) const
{
	if (face == RubikFace_PosX || face == RubikFace_NegX)
		return face;
	while (times--)
	{
		switch (face)
		{
		case RubikFace_PosY: face = RubikFace_NegZ; break;
		case RubikFace_PosZ: face = RubikFace_PosY; break;
		case RubikFace_NegY: face = RubikFace_PosZ; break;
		case RubikFace_NegZ: face = RubikFace_NegY; break;
		}
	}
	return face;
}

RubikFace Rubik::GetTargetSwapFaceRotationY(RubikFace face, int times) const
{
	if (face == RubikFace_PosY || face == RubikFace_NegY)
		return face;
	while (times--)
	{
		switch (face)
		{
		case RubikFace_PosZ: face = RubikFace_NegX; break;
		case RubikFace_PosX: face = RubikFace_PosZ; break;
		case RubikFace_NegZ: face = RubikFace_PosX; break;
		case RubikFace_NegX: face = RubikFace_NegZ; break;
		}
	}
	return face;
}

RubikFace Rubik::GetTargetSwapFaceRotationZ(RubikFace face, int times) const
{
	if (face == RubikFace_PosZ || face == RubikFace_NegZ)
		return face;
	while (times--)
	{
		switch (face)
		{
		case RubikFace_PosX: face = RubikFace_NegY; break;
		case RubikFace_PosY: face = RubikFace_PosX; break;
		case RubikFace_NegX: face = RubikFace_PosY; break;
		case RubikFace_NegY: face = RubikFace_NegX; break;
		}
	}
	return face;
}
