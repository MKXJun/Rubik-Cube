#include "Rubik.h"
#include "d3dUtil.h"
#include "Vertex.h"
using namespace DirectX;
using namespace Microsoft::WRL;



DirectX::XMMATRIX Cube::GetWorldMatrix() const
{
	XMVECTOR posVec = XMLoadFloat3(&pos);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	posVec = XMVector3TransformCoord(posVec, R);
	XMFLOAT3 finalPos;
	XMStoreFloat3(&finalPos, posVec);

	return XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) *
		XMMatrixTranslation(finalPos.x, finalPos.y, finalPos.z);
}

void Rubik::InitResources(ComPtr<ID3D11Device> device, ComPtr<ID3D11DeviceContext> deviceContext)
{
	// 初始化纹理数组
	mTexArray = CreateDDSTexture2DArrayFromFile(
		device,
		deviceContext,
		std::vector<std::wstring>{
			L"Resource/Black.dds",
			L"Resource/Orange.dds",
			L"Resource/Red.dds",
			L"Resource/Green.dds",
			L"Resource/Blue.dds",
			L"Resource/Yellow.dds",
			L"Resource/White.dds",
	});

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
	// 初始化魔方中心位置，用六个面默认填充黑色
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
			for (int k = 0; k < 3; ++k)
			{
				mCubes[i][j][k].pos = XMFLOAT3(-2.0f + 2.0f * i,
					-2.0f + 2.0f * j, -2.0f + 2.0f * k);
				mCubes[i][j][k].rotation = XMFLOAT3();
				memset(mCubes[i][j][k].faceColors, 0, 
					sizeof mCubes[i][j][k].faceColors);
			}
	
	

	// +X面为橙色，-X面为红色
	for (int j = 0; j < 3; ++j)
		for (int k = 0; k < 3; ++k)
		{
			mCubes[2][j][k].faceColors[RubikFace_PosX] = RubikFaceColor_Orange;
			mCubes[0][j][k].faceColors[RubikFace_NegX] = RubikFaceColor_Red;
		}	

	// +Y面为绿色，-Y面为蓝色
	for (int i = 0; i < 3; ++i)
		for (int k = 0; k < 3; ++k)
		{
			mCubes[i][2][k].faceColors[RubikFace_PosY] = RubikFaceColor_Green;
			mCubes[i][0][k].faceColors[RubikFace_NegY] = RubikFaceColor_Blue;
		}

	// +Z面为黄色，-Z面为白色
	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
		{
			mCubes[i][j][2].faceColors[RubikFace_PosZ] = RubikFaceColor_Yellow;
			mCubes[i][j][0].faceColors[RubikFace_NegZ] = RubikFaceColor_White;
		}

}

void Rubik::Update()
{
}

void Rubik::Draw(ComPtr<ID3D11DeviceContext> deviceContext, BasicEffect& effect)
{
	/*static float rot = 0.0f;
	rot += 0.0005f;
	for (int i = 0; i < 3; ++i)
		for (int k = 0; k < 3; ++k)
		{
			mCubes[i][2][k].rotation.y = rot;
		}*/

	for (int i = 0; i < 3; ++i)
		for (int j = 0; j < 3; ++j)
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

ComPtr<ID3D11ShaderResourceView> Rubik::GetTexArray() const
{
	return mTexArray;
}
