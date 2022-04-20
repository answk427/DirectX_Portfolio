//***************************************************************************************
// ShadowMap.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "ShadowMap.h"

ShadowMap::ShadowMap(ID3D11Device* device, UINT width, UINT height, const std::wstring& shaderName)
	: mWidth(width), mHeight(height), mDepthMapSRV(0), mDepthMapDSV(0), m_shaderName(shaderName),
	 m_shadowMapCamera(make_unique<Camera>())
{

	EffectMgr& effectMgr = EffectMgr::Instance();
	effectMgr.SetType(m_shaderName, EffectType::BuildShadowMap);

	mViewport.TopLeftX = 0.0f;
	mViewport.TopLeftY = 0.0f;
	mViewport.Width = static_cast<float>(width);
	mViewport.Height = static_cast<float>(height);
	mViewport.MinDepth = 0.0f;
	mViewport.MaxDepth = 1.0f;

	// Use typeless format because the DSV is going to interpret
	// the bits as DXGI_FORMAT_D24_UNORM_S8_UINT, whereas the SRV is going to interpret
	// the bits as DXGI_FORMAT_R24_UNORM_X8_TYPELESS.
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = mWidth;
	texDesc.Height = mHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	ID3D11Texture2D* depthMap = 0;
	HR(device->CreateTexture2D(&texDesc, 0, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	HR(device->CreateDepthStencilView(depthMap, &dsvDesc, &mDepthMapDSV));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	HR(device->CreateShaderResourceView(depthMap, &srvDesc, &mDepthMapSRV));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(depthMap);
}

ShadowMap::~ShadowMap()
{
	ReleaseCOM(mDepthMapSRV);
	ReleaseCOM(mDepthMapDSV);
}

ID3D11ShaderResourceView* ShadowMap::DepthMapSRV()
{
	return mDepthMapSRV;
}

void ShadowMap::BindDsvAndSetNullRenderTarget(ID3D11DeviceContext* dc)
{
	dc->RSSetViewports(1, &mViewport);

	// Set null render target because we are only going to draw to depth buffer.
	// Setting a null render target will disable color writes.
	ID3D11RenderTargetView* renderTargets[1] = { 0 };
	dc->OMSetRenderTargets(1, renderTargets, mDepthMapDSV);

	dc->ClearDepthStencilView(mDepthMapDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void ShadowMap::ComputeBoundingSphere(std::vector<Renderer*> renderers)
{
	XMFLOAT3 maxPosF(-MathHelper::Infinity, -MathHelper::Infinity, -MathHelper::Infinity);
	XMFLOAT3 minPosF(MathHelper::Infinity, MathHelper::Infinity, MathHelper::Infinity);

	XMVECTOR maxPosV = XMLoadFloat3(&maxPosF);
	XMVECTOR minPosV = XMLoadFloat3(&minPosF);


	for (auto renderer : renderers)
	{
		Mesh* mesh = renderer->GetMesh();
		XMMATRIX world = XMLoadFloat4x4(renderer->GetTransform()->GetWorld());
		
		//모든 정점 중에 최소정점과 최대정점을 구함.
		auto& vertices = mesh->GetVertices();
		for (auto& vertex : vertices)
		{
			//월드좌표에서의 정점위치
			XMVECTOR P = XMVector3TransformCoord(XMLoadFloat3(&vertex.pos), world);
			maxPosV = XMVectorMax(maxPosV, P);
			minPosV = XMVectorMin(minPosV, P);
		}

	}


	XMVECTOR centerV = 0.5f * (maxPosV + minPosV);
	XMStoreFloat3(&m_boundingSphere.center, centerV);
	m_boundingSphere.radius = XMVectorGetX(XMVector3Length(centerV - minPosV));
}

bool ShadowMap::BuildShadowTransform(const DirectionalLight & dirLight, XMVECTOR& worldUp)
{
	//평행광의 방향
	static XMVECTOR preLightDir = XMLoadFloat3(&dirLight.Direction);
	XMVECTOR lightDir = XMLoadFloat3(&dirLight.Direction);
	
	//방향의 변화가 없으면 굳이 계산할 필요없음	
	if (XMVector3Equal(preLightDir, lightDir))
		return false;
	
	//바라보는 점
	XMVECTOR targetPos = XMLoadFloat3(&m_boundingSphere.center);
	//평행광원의 위치(경계구의 중점에서 평행광방향의 반대방향*2 만큼 이동한 점)
	XMVECTOR lightPos = targetPos - 2.0f*m_boundingSphere.radius*lightDir;
	XMFLOAT3 lightPosF;
	XMStoreFloat3(&lightPosF, lightPos);
	m_shadowMapCamera->SetPosition(lightPosF);

	//시야행렬
	XMMATRIX view = XMMatrixLookAtLH(lightPos, targetPos, worldUp);
	XMFLOAT4X4 tempView;
	XMStoreFloat4x4(&tempView, view);
	m_shadowMapCamera->SetView(tempView);

	//광원공간에서 경계구의 원점 위치
	XMFLOAT3 viewTargetPos;
	XMStoreFloat3(&viewTargetPos,XMVector3TransformCoord(targetPos, view));

	m_shadowMapCamera->SetOrthographicLens(viewTargetPos, m_boundingSphere.radius);

	// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
	XMMATRIX ndc2Tex(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	//XMMATRIX S = m_shadowMapCamera->ViewProj() *ndc2Tex;
	XMMATRIX S = m_shadowMapCamera->ViewProj() *ndc2Tex;

	XMStoreFloat4x4(&mShadowTransform, S);

	return true;
}

