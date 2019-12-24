#include "Effects.h"
#include "d3dUtil.h"
#include "EffectHelper.h"	// 必须晚于Effects.h和d3dUtil.h包含
#include "Vertex.h"
using namespace DirectX;

// 着色器字节码，编译后产生
#include "HLSL/Basic_VS.inc"
#include "HLSL/Basic_PS.inc"


//
// BasicEffect::Impl 需要先于BasicEffect的定义
//

class BasicEffect::Impl : public AlignedType<BasicEffect::Impl>
{
public:

	//
	// 这些结构体对应HLSL的结构体。需要按16字节对齐
	//

	struct CBChangesEveryDrawing
	{
		int texIndex;
		XMFLOAT3 gPad;
	};

	struct CBChangesEveryCube
	{
		XMMATRIX world;
	};

	struct CBChangesEveryFrame
	{
		XMMATRIX view;
	};

	struct CBChangesOnResize
	{
		XMMATRIX proj;
	};

public:
	// 必须显式指定
	Impl() = default;
	~Impl() = default;

public:
	// 需要16字节对齐的优先放在前面
	CBufferObject<0, CBChangesEveryDrawing> cbDrawing;		// 每次对象绘制的常量缓冲区
	CBufferObject<1, CBChangesEveryCube>    cbCube;			// 每个立方体绘制的常量缓冲区
	CBufferObject<2, CBChangesEveryFrame>   cbFrame;		// 每帧绘制的常量缓冲区
	CBufferObject<3, CBChangesOnResize>     cbOnResize;		// 每次窗口大小变更的常量缓冲区
	BOOL isDirty;											// 是否有值变更
	std::vector<CBufferBase*> cBufferPtrs;					// 统一管理上面所有的常量缓冲区


	ComPtr<ID3D11VertexShader> basicVS;						// 顶点着色器
	ComPtr<ID3D11PixelShader>  basicPS;						// 像素着色器

	ComPtr<ID3D11SamplerState> ssLinearWrap;				// 线性采样器状态

	ComPtr<ID3D11InputLayout>  vertexLayout;				// 顶点输入布局

	ComPtr<ID3D11ShaderResourceView> textureArray;			// 用于绘制的纹理数组

};

//
// BasicEffect
//

namespace
{
	// BasicEffect单例
	static BasicEffect * pInstance = nullptr;
}

BasicEffect::BasicEffect()
{
	if (pInstance)
		throw std::exception("BasicEffect is a singleton!");
	pInstance = this;
	pImpl = std::make_unique<BasicEffect::Impl>();
}

BasicEffect::~BasicEffect()
{
}

BasicEffect::BasicEffect(BasicEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
}

BasicEffect & BasicEffect::operator=(BasicEffect && moveFrom)
{
	pImpl.swap(moveFrom.pImpl);
	return *this;
}

BasicEffect & BasicEffect::Get()
{
	if (!pInstance)
		throw std::exception("BasicEffect needs an instance!");
	return *pInstance;
}


bool BasicEffect::InitAll(ComPtr<ID3D11Device> device)
{
	if (!device)
		return false;

	if (!pImpl->cBufferPtrs.empty())
		return true;

	// 创建顶点着色器
	HR(device->CreateVertexShader(g_Basic_VS, sizeof(g_Basic_VS), nullptr, pImpl->basicVS.GetAddressOf()));
	// 创建顶点布局
	HR(device->CreateInputLayout(VertexPosTex::inputLayout, ARRAYSIZE(VertexPosTex::inputLayout),
		g_Basic_VS, sizeof(g_Basic_VS), pImpl->vertexLayout.GetAddressOf()));

	// 创建像素着色器
	HR(device->CreatePixelShader(g_Basic_PS, sizeof(g_Basic_PS), nullptr, pImpl->basicPS.GetAddressOf()));

	// 创建采样器状态
	D3D11_SAMPLER_DESC sd;
	ZeroMemory(&sd, sizeof sd);
	sd.AddressU = sd.AddressV = sd.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sd.Filter = D3D11_FILTER_ANISOTROPIC;
	sd.MinLOD = 0;
	sd.MaxLOD = D3D11_FLOAT32_MAX;
	HR(device->CreateSamplerState(&sd, pImpl->ssLinearWrap.GetAddressOf()));

	pImpl->cBufferPtrs.assign({
		&pImpl->cbDrawing, 
		&pImpl->cbCube,
		&pImpl->cbFrame, 
		&pImpl->cbOnResize});

	// 创建常量缓冲区
	for (auto& pBuffer : pImpl->cBufferPtrs)
	{
		pBuffer->CreateBuffer(device);
	}

	return true;
}

void BasicEffect::SetRenderDefault(ComPtr<ID3D11DeviceContext> deviceContext)
{
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(pImpl->vertexLayout.Get());
	deviceContext->VSSetShader(pImpl->basicVS.Get(), nullptr, 0);
	deviceContext->RSSetState(nullptr);
	deviceContext->PSSetShader(pImpl->basicPS.Get(), nullptr, 0);
	deviceContext->PSSetSamplers(0, 1, pImpl->ssLinearWrap.GetAddressOf());
	deviceContext->OMSetDepthStencilState(nullptr, 0);
	deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);
}

void XM_CALLCONV BasicEffect::SetWorldMatrix(DirectX::FXMMATRIX W)
{
	auto& cBuffer = pImpl->cbCube;
	cBuffer.data.world = XMMatrixTranspose(W);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetViewMatrix(FXMMATRIX V)
{
	auto& cBuffer = pImpl->cbFrame;
	cBuffer.data.view = XMMatrixTranspose(V);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetProjMatrix(FXMMATRIX P)
{
	auto& cBuffer = pImpl->cbOnResize;
	cBuffer.data.proj = XMMatrixTranspose(P);
	pImpl->isDirty = cBuffer.isDirty = true;
}

void XM_CALLCONV BasicEffect::SetWorldViewProjMatrix(FXMMATRIX W, CXMMATRIX V, CXMMATRIX P)
{
	pImpl->cbCube.data.world = XMMatrixTranspose(W);
	pImpl->cbFrame.data.view = XMMatrixTranspose(V);
	pImpl->cbOnResize.data.proj = XMMatrixTranspose(P);

	auto& pCBuffers = pImpl->cBufferPtrs;
	pCBuffers[1]->isDirty = pCBuffers[2]->isDirty = pCBuffers[3]->isDirty = true;
	pImpl->isDirty = true;
}

void BasicEffect::SetTextureArray(ComPtr<ID3D11ShaderResourceView> textureArray)
{
	pImpl->textureArray = textureArray;
}

void BasicEffect::SetTexIndex(int index)
{
	auto& cBuffer = pImpl->cbDrawing;
	cBuffer.data.texIndex = index;
	pImpl->isDirty = cBuffer.isDirty = true;
}

void BasicEffect::Apply(ComPtr<ID3D11DeviceContext> deviceContext)
{
	auto& pCBuffers = pImpl->cBufferPtrs;
	// 将缓冲区绑定到渲染管线上
	pCBuffers[1]->BindVS(deviceContext);
	pCBuffers[2]->BindVS(deviceContext);
	pCBuffers[3]->BindVS(deviceContext);

	pCBuffers[0]->BindPS(deviceContext);

	// 设置纹理
	deviceContext->PSSetShaderResources(0, 1, pImpl->textureArray.GetAddressOf());

	if (pImpl->isDirty)
	{
		pImpl->isDirty = false;
		for (auto& pCBuffer : pCBuffers)
		{
			pCBuffer->UpdateBuffer(deviceContext);
		}
	}
}




