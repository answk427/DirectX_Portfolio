//***************************************************************************************
// Effects.cpp by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************

#include "Effects.h"

XMMATRIX g_shadowMatrix;
void SetShadowMatrix(XMMATRIX shadowMatrix) { g_shadowMatrix = shadowMatrix; }

ID3D11ShaderResourceView* g_shadowSRV = nullptr;
void SetShadowSRV(ID3D11ShaderResourceView * shadowSRV)
{
	g_shadowSRV = shadowSRV;
}

#pragma region Effect
Effect::Effect(ID3D11Device* device, const std::wstring& filename)
	: mFX(0), m_inputLayout(0), m_blendState(0), m_instancing_inputLayout(0)
{
	std::ifstream fin(filename.c_str(), std::ios::binary);

	fin.seekg(0, std::ios_base::end);
	int size = (int)fin.tellg();
	fin.seekg(0, std::ios_base::beg);
	std::vector<char> compiledShader(size);

	fin.read(&compiledShader[0], size);
	fin.close();

	HR(D3DX11CreateEffectFromMemory(&compiledShader[0], size,
		0, device, &mFX));
}

Effect::~Effect()
{
	ReleaseCOM(mFX);
	ReleaseCOM(m_inputLayout);
	ReleaseCOM(m_blendState);
}

void Effect::Init(ID3D11Device* device)
{
	InitInputLayout(device);
	InitInstancingInputLayout(device);
	InitSpecialInputLayout(device);
	InitBlendState(device);
}

void Effect::InitBlendState(ID3D11Device * device)
{
	D3D11_BLEND_DESC blendDesc = { 0 };
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;

	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//alpha 성분 혼합계수 설정
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	ReleaseCOM(m_blendState);
	HR(device->CreateBlendState(&blendDesc, &m_blendState));
}


#pragma endregion

#pragma region BasicEffect
BasicEffect::BasicEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename), m_skinning_inputLayout(0), m_skinning_instancing_inputLayout(0)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");

	Light3TexInstancingTech = mFX->GetTechniqueByName("Light3TexInstancing");
	Light3TexSkinningTech = mFX->GetTechniqueByName("Light3TexSkinning");
	Light3TexSkinningInstancingTech = mFX->GetTechniqueByName("Light3TexSkinningInstancing");

	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");
	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech = mFX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech = mFX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech = mFX->GetTechniqueByName("Light3Reflect");

	Light0TexReflectTech = mFX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = mFX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = mFX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = mFX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech = mFX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech = mFX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech = mFX->GetTechniqueByName("Light3FogReflect");

	Light0TexFogReflectTech = mFX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = mFX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = mFX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = mFX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	ShadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	isShadowed = mFX->GetVariableByName("isShadowed")->AsScalar();

	DirLights = mFX->GetVariableByName("gDirLights");
	dirLightSize = mFX->GetVariableByName("dirLight_size")->AsScalar();

	pointLights = mFX->GetVariableByName("gPointLights");
	pointLightSize = mFX->GetVariableByName("pointLight_size")->AsScalar();

	spotLights = mFX->GetVariableByName("gSpotLights");
	spotLightSize = mFX->GetVariableByName("spotLight_size")->AsScalar();

	Mat = mFX->GetVariableByName("gMaterial");
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	ShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
	SsaoMap = mFX->GetVariableByName("gSsaoMap")->AsShaderResource();
	DiffuseMapArray = mFX->GetVariableByName("gDiffuseMapArray")->AsShaderResource();

	BoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();

	//
	Init(device);
}

BasicEffect::~BasicEffect()
{
}
void BasicEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{
	//쉐이더에 조명설정
	SetDirLights(directL);
	SetPointLights(pointL);
	SetSpotLights(spotL);

	//그림자 맵 설정
	SetShadowMap(g_shadowSRV);

	SetEyePosW(camera.GetPosition());
}

void BasicEffect::PerObjectSet(GeneralMaterial * material,
	Camera * camera,
	CXMMATRIX& world)
{
	SetWorld(world);

	//비균등 비례로 인한 법선벡터 계산에 쓰이는 행렬
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	SetWorldInvTranspose(worldInvTranspose);

	//물체공간 -> 투영공간 변환행렬
	XMMATRIX worldViewProj = world * camera->ViewProj();
	SetWorldViewProj(worldViewProj);

	//인스턴스의 세계행렬과 곱해질 시야투영행렬
	SetViewProj(camera->ViewProj());

	//텍스쳐 변환 I * S * (R) * T
	XMMATRIX texTransform = XMMatrixIdentity() *
		XMMatrixScaling(material->textureTiling.x,
			material->textureTiling.y, 0.0f) *
		XMMatrixTranslation(material->textureOffset.x,
			material->textureOffset.y, 0.0f);
	SetTexTransform(texTransform);

	//세계공간 -> 광원의 ndc좌표 -> 텍스쳐공간 좌표 변환 행렬
	SetShadowTransform(world * g_shadowMatrix);

	//재질 설정
	SetMaterial(material->basicMat);
}
void BasicEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{
	//if(diffuseMap!=nullptr)
	SetDiffuseMap(diffuseMap);
}
void BasicEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{
	DiffuseMapArray->SetResource(arr);
}
void BasicEffect::SetBoneTransforms(const XMFLOAT4X4 * M, int cnt)
{
	BoneTransforms->SetMatrixArray(reinterpret_cast<const float*>(M), 0, cnt); 
}
ID3DX11EffectTechnique * BasicEffect::GetTechnique(UINT techType)
{
	if (techType & TechniqueType::Shadowed)
		SetIsShadowed(true);
	else
		SetIsShadowed(false);

	techType = techType & ~TechniqueType::Shadowed;

	switch (techType)
	{

	case TechniqueType::Light:
		return Light3Tech;
	case TechniqueType::Light | TechniqueType::DiffuseMap:
		return Light3TexTech;
	case TechniqueType::Light | TechniqueType::DiffuseMap | TechniqueType::Skinned:
		return Light3TexSkinningTech;
	case TechniqueType::Light | TechniqueType::DiffuseMap | TechniqueType::Instancing:
		return Light3TexInstancingTech;
	case TechniqueType::Light | TechniqueType::DiffuseMap | TechniqueType::Instancing |
		TechniqueType::Skinned:
		return Light3TexSkinningInstancingTech;
	default:
		nullptr;
	}
	return nullptr;
}

void BasicEffect::InitInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_inputLayout);
	D3DX11_PASS_DESC passDesc;

	Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));
}
void BasicEffect::InitSpecialInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_skinning_inputLayout);
	ReleaseCOM(m_skinning_instancing_inputLayout);
	D3DX11_PASS_DESC passDesc;

	Light3TexSkinningTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32Skinned, 5, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_skinning_inputLayout));

	Light3TexSkinningInstancingTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Basic32SkinnedInstancing, 15, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_skinning_instancing_inputLayout));
}
bool BasicEffect::IASetting(ID3D11DeviceContext * context, UINT techType)
{
	//instancing으로 렌더링 할 때
	if (techType & TechniqueType::Instancing)
	{
		if (techType & TechniqueType::Skinned)
		{
			if (m_skinning_instancing_inputLayout == nullptr)
				return false;
			context->IASetInputLayout(m_skinning_instancing_inputLayout);
		}
		else
		{
			if (m_instancing_inputLayout == nullptr)
				return false;
			context->IASetInputLayout(m_instancing_inputLayout);
		}
	}
	else
	{
		if (techType & TechniqueType::Skinned)
		{
			if (m_skinning_inputLayout == nullptr)
				return false;
			context->IASetInputLayout(m_skinning_inputLayout);
		}
		else
		{
			if (m_inputLayout == nullptr)
				return false;
			context->IASetInputLayout(m_inputLayout);
		}
	}
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}
void BasicEffect::InitInstancingInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_instancing_inputLayout);

	D3DX11_PASS_DESC passDesc;

	//입력서명
	Light3TexInstancingTech->GetPassByIndex(0)->GetDesc(&passDesc);

	HR(device->CreateInputLayout(InputLayoutDesc::Basic32Instancing, 13, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_instancing_inputLayout));
}

#pragma endregion

#pragma region NormalMapEffect
NormalMapEffect::NormalMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");

	Light0TexTech = mFX->GetTechniqueByName("Light0Tex");
	Light1TexTech = mFX->GetTechniqueByName("Light1Tex");
	Light2TexTech = mFX->GetTechniqueByName("Light2Tex");
	Light3TexTech = mFX->GetTechniqueByName("Light3Tex");


	Light0TexAlphaClipTech = mFX->GetTechniqueByName("Light0TexAlphaClip");
	Light1TexAlphaClipTech = mFX->GetTechniqueByName("Light1TexAlphaClip");
	Light2TexAlphaClipTech = mFX->GetTechniqueByName("Light2TexAlphaClip");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");

	Light1FogTech = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech = mFX->GetTechniqueByName("Light3Fog");

	Light0TexFogTech = mFX->GetTechniqueByName("Light0TexFog");
	Light1TexFogTech = mFX->GetTechniqueByName("Light1TexFog");
	Light2TexFogTech = mFX->GetTechniqueByName("Light2TexFog");

	Light3TexFogTech = mFX->GetTechniqueByName("Light3TexFog");

	Light0TexAlphaClipFogTech = mFX->GetTechniqueByName("Light0TexAlphaClipFog");
	Light1TexAlphaClipFogTech = mFX->GetTechniqueByName("Light1TexAlphaClipFog");
	Light2TexAlphaClipFogTech = mFX->GetTechniqueByName("Light2TexAlphaClipFog");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	Light1ReflectTech = mFX->GetTechniqueByName("Light1Reflect");
	Light2ReflectTech = mFX->GetTechniqueByName("Light2Reflect");
	Light3ReflectTech = mFX->GetTechniqueByName("Light3Reflect");

	Light0TexReflectTech = mFX->GetTechniqueByName("Light0TexReflect");
	Light1TexReflectTech = mFX->GetTechniqueByName("Light1TexReflect");
	Light2TexReflectTech = mFX->GetTechniqueByName("Light2TexReflect");
	Light3TexReflectTech = mFX->GetTechniqueByName("Light3TexReflect");

	Light0TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflect");
	Light1TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflect");
	Light2TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflect");
	Light3TexAlphaClipReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflect");

	Light1FogReflectTech = mFX->GetTechniqueByName("Light1FogReflect");
	Light2FogReflectTech = mFX->GetTechniqueByName("Light2FogReflect");
	Light3FogReflectTech = mFX->GetTechniqueByName("Light3FogReflect");

	Light0TexFogReflectTech = mFX->GetTechniqueByName("Light0TexFogReflect");
	Light1TexFogReflectTech = mFX->GetTechniqueByName("Light1TexFogReflect");
	Light2TexFogReflectTech = mFX->GetTechniqueByName("Light2TexFogReflect");
	Light3TexFogReflectTech = mFX->GetTechniqueByName("Light3TexFogReflect");

	Light0TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflect");
	Light1TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflect");
	Light2TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflect");
	Light3TexAlphaClipFogReflectTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflect");

	Light1SkinnedTech = mFX->GetTechniqueByName("Light1Skinned");
	Light2SkinnedTech = mFX->GetTechniqueByName("Light2Skinned");
	Light3SkinnedTech = mFX->GetTechniqueByName("Light3Skinned");

	Light0TexSkinnedTech = mFX->GetTechniqueByName("Light0TexSkinned");
	Light1TexSkinnedTech = mFX->GetTechniqueByName("Light1TexSkinned");
	Light2TexSkinnedTech = mFX->GetTechniqueByName("Light2TexSkinned");
	Light3TexSkinnedTech = mFX->GetTechniqueByName("Light3TexSkinned");

	Light0TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipSkinned");
	Light1TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipSkinned");
	Light2TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipSkinned");
	Light3TexAlphaClipSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipSkinned");

	Light1FogSkinnedTech = mFX->GetTechniqueByName("Light1FogSkinned");
	Light2FogSkinnedTech = mFX->GetTechniqueByName("Light2FogSkinned");
	Light3FogSkinnedTech = mFX->GetTechniqueByName("Light3FogSkinned");

	Light0TexFogSkinnedTech = mFX->GetTechniqueByName("Light0TexFogSkinned");
	Light1TexFogSkinnedTech = mFX->GetTechniqueByName("Light1TexFogSkinned");
	Light2TexFogSkinnedTech = mFX->GetTechniqueByName("Light2TexFogSkinned");
	Light3TexFogSkinnedTech = mFX->GetTechniqueByName("Light3TexFogSkinned");

	Light0TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogSkinned");
	Light1TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogSkinned");
	Light2TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogSkinned");
	Light3TexAlphaClipFogSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogSkinned");

	Light1ReflectSkinnedTech = mFX->GetTechniqueByName("Light1ReflectSkinned");
	Light2ReflectSkinnedTech = mFX->GetTechniqueByName("Light2ReflectSkinned");
	Light3ReflectSkinnedTech = mFX->GetTechniqueByName("Light3ReflectSkinned");

	Light0TexReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexReflectSkinned");
	Light1TexReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexReflectSkinned");
	Light2TexReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexReflectSkinned");
	Light3TexReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexReflectSkinned");

	Light0TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipReflectSkinned");
	Light1TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipReflectSkinned");
	Light2TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipReflectSkinned");
	Light3TexAlphaClipReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipReflectSkinned");

	Light1FogReflectSkinnedTech = mFX->GetTechniqueByName("Light1FogReflectSkinned");
	Light2FogReflectSkinnedTech = mFX->GetTechniqueByName("Light2FogReflectSkinned");
	Light3FogReflectSkinnedTech = mFX->GetTechniqueByName("Light3FogReflectSkinned");

	Light0TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexFogReflectSkinned");
	Light1TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexFogReflectSkinned");
	Light2TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexFogReflectSkinned");
	Light3TexFogReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexFogReflectSkinned");

	Light0TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light0TexAlphaClipFogReflectSkinned");
	Light1TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light1TexAlphaClipFogReflectSkinned");
	Light2TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light2TexAlphaClipFogReflectSkinned");
	Light3TexAlphaClipFogReflectSkinnedTech = mFX->GetTechniqueByName("Light3TexAlphaClipFogReflectSkinned");

	//ViewProj		  = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	WorldViewProjTex = mFX->GetVariableByName("gWorldViewProjTex")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	BoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
	ShadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
	ShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
	SsaoMap = mFX->GetVariableByName("gSsaoMap")->AsShaderResource();

	//
	Init(device);

}

NormalMapEffect::~NormalMapEffect()
{
}

void NormalMapEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{
}

void NormalMapEffect::PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world)
{
}

ID3DX11EffectTechnique * NormalMapEffect::GetTechnique(UINT techType)
{
	return nullptr;
}



void NormalMapEffect::InitInputLayout(ID3D11Device * device)
{
}

void NormalMapEffect::InitInstancingInputLayout(ID3D11Device * device)
{
}

void NormalMapEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{
}

void NormalMapEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{
}

#pragma endregion

#pragma region BuildShadowMapEffect
BuildShadowMapEffect::BuildShadowMapEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename), m_terrainLayout(0)
{
	BuildShadowMapTech = mFX->GetTechniqueByName("BuildShadowMapTech");
	BuildShadowMapAlphaClipTech = mFX->GetTechniqueByName("BuildShadowMapAlphaClipTech");

	BuildShadowMapSkinnedTech = mFX->GetTechniqueByName("BuildShadowMapSkinnedTech");
	BuildShadowMapAlphaClipSkinnedTech = mFX->GetTechniqueByName("BuildShadowMapAlphaClipSkinnedTech");

	TessBuildShadowMapTech = mFX->GetTechniqueByName("TessBuildShadowMapTech");
	TessBuildShadowMapAlphaClipTech = mFX->GetTechniqueByName("TessBuildShadowMapAlphaClipTech");
	TerrainTech = mFX->GetTechniqueByName("TerrainBuildShadowMapAlphaClipTech");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();
	BoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	HeightScale = mFX->GetVariableByName("gHeightScale")->AsScalar();
	MaxTessDistance = mFX->GetVariableByName("gMaxTessDistance")->AsScalar();
	MinTessDistance = mFX->GetVariableByName("gMinTessDistance")->AsScalar();
	MinTessFactor = mFX->GetVariableByName("gMinTessFactor")->AsScalar();
	MaxTessFactor = mFX->GetVariableByName("gMaxTessFactor")->AsScalar();
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	DiffuseMapArray = mFX->GetVariableByName("gDiffuseMapArray")->AsShaderResource();
	NormalMap = mFX->GetVariableByName("gNormalMap")->AsShaderResource();
	HeightMap = mFX->GetVariableByName("gHeightMap")->AsShaderResource();
	
	Init(device);
}

BuildShadowMapEffect::~BuildShadowMapEffect()
{
}
void BuildShadowMapEffect::InitInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_inputLayout);
	D3DX11_PASS_DESC passDesc;

	//HR(BuildShadowMapTech->GetPassByIndex(0)->GetDesc(&passDesc));
	HR(BuildShadowMapAlphaClipTech->GetPassByIndex(0)->GetDesc(&passDesc));

	HR(device->CreateInputLayout(InputLayoutDesc::PosNormalTexTan, 4, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));

	ReleaseCOM(m_terrainLayout);
	HR(TerrainTech->GetPassByIndex(0)->GetDesc(&passDesc));
	HR(device->CreateInputLayout(InputLayoutDesc::Terrain, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_terrainLayout));
}
void BuildShadowMapEffect::InitInstancingInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_inputLayout);
	D3DX11_PASS_DESC passDesc;

	BuildShadowMapTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::PosNormalTexTan, 4, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));
}

void BuildShadowMapEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{
	//쉐이더에 조명설정
	//SetEyePosW(camera.GetPosition());

	//인스턴스의 세계행렬과 곱해질 시야투영행렬
	SetViewProj(camera.ViewProj());
}
void BuildShadowMapEffect::PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world)
{
	SetWorld(world);

	SetEyePosW(camera->GetPosition());
	//물체공간 -> 투영공간 변환행렬
	//XMMATRIX worldViewProj = world * camera->ViewProj();
	//SetWorldViewProj(worldViewProj);

	//비균등 비례로 인한 법선벡터 계산에 쓰이는 행렬
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	SetWorldInvTranspose(worldInvTranspose);

	//텍스쳐 변환 I * S * (R) * T
	XMMATRIX texTransform = XMMatrixIdentity() *
		XMMatrixScaling(material->textureTiling.x,
			material->textureTiling.y, 0.0f) *
		XMMatrixTranslation(material->textureOffset.x,
			material->textureOffset.y, 0.0f);
	SetTexTransform(texTransform);
}
ID3DX11EffectTechnique * BuildShadowMapEffect::GetTechnique(UINT techType)
{
	if (techType & TechniqueType::Tesselation)
		return TerrainTech;
	techType = techType & ~TechniqueType::Shadowed;

	switch (techType)
	{
	case TechniqueType::Light:
		return BuildShadowMapTech;
	case TechniqueType::Light | TechniqueType::DiffuseMap:
		//return BuildShadowMapAlphaClipTech;
		return BuildShadowMapTech;
	case TechniqueType::Light | TechniqueType::DiffuseMap | TechniqueType::Instancing:
		return BuildShadowMapTech;
	default:
		return BuildShadowMapTech;
	}
	return nullptr;
}
void BuildShadowMapEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{
	SetDiffuseMap(diffuseMap);
}
void BuildShadowMapEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{
	SetDiffuseMapArray(arr);
}
bool BuildShadowMapEffect::IASetting(ID3D11DeviceContext * context, UINT techType)
{
	//instancing으로 렌더링 할 때
	if (techType & TechniqueType::Instancing)
	{
		if (m_instancing_inputLayout == nullptr)
			return false;
		context->IASetInputLayout(m_instancing_inputLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else if (techType & TechniqueType::Tesselation)
	{
		if (m_terrainLayout == nullptr)
			return false;
		context->IASetInputLayout(m_terrainLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	}
	else 
	{
		if (m_inputLayout == nullptr)
			return false;
		context->IASetInputLayout(m_inputLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	}


	return true;
}
#pragma endregion

#pragma region SsaoNormalDepthEffect
SsaoNormalDepthEffect::SsaoNormalDepthEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	NormalDepthTech = mFX->GetTechniqueByName("NormalDepth");
	NormalDepthAlphaClipTech = mFX->GetTechniqueByName("NormalDepthAlphaClip");

	NormalDepthSkinnedTech = mFX->GetTechniqueByName("NormalDepthSkinned");
	NormalDepthAlphaClipSkinnedTech = mFX->GetTechniqueByName("NormalDepthAlphaClipSkinned");

	WorldView = mFX->GetVariableByName("gWorldView")->AsMatrix();
	WorldInvTransposeView = mFX->GetVariableByName("gWorldInvTransposeView")->AsMatrix();
	BoneTransforms = mFX->GetVariableByName("gBoneTransforms")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
}

SsaoNormalDepthEffect::~SsaoNormalDepthEffect()
{
}
#pragma endregion

#pragma region SsaoEffect
SsaoEffect::SsaoEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SsaoTech = mFX->GetTechniqueByName("Ssao");

	ViewToTexSpace = mFX->GetVariableByName("gViewToTexSpace")->AsMatrix();
	OffsetVectors = mFX->GetVariableByName("gOffsetVectors")->AsVector();
	FrustumCorners = mFX->GetVariableByName("gFrustumCorners")->AsVector();
	OcclusionRadius = mFX->GetVariableByName("gOcclusionRadius")->AsScalar();
	OcclusionFadeStart = mFX->GetVariableByName("gOcclusionFadeStart")->AsScalar();
	OcclusionFadeEnd = mFX->GetVariableByName("gOcclusionFadeEnd")->AsScalar();
	SurfaceEpsilon = mFX->GetVariableByName("gSurfaceEpsilon")->AsScalar();

	NormalDepthMap = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	RandomVecMap = mFX->GetVariableByName("gRandomVecMap")->AsShaderResource();
}

SsaoEffect::~SsaoEffect()
{
}
#pragma endregion

#pragma region SsaoBlurEffect
SsaoBlurEffect::SsaoBlurEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	HorzBlurTech = mFX->GetTechniqueByName("HorzBlur");
	VertBlurTech = mFX->GetTechniqueByName("VertBlur");

	TexelWidth = mFX->GetVariableByName("gTexelWidth")->AsScalar();
	TexelHeight = mFX->GetVariableByName("gTexelHeight")->AsScalar();

	NormalDepthMap = mFX->GetVariableByName("gNormalDepthMap")->AsShaderResource();
	InputImage = mFX->GetVariableByName("gInputImage")->AsShaderResource();
}

SsaoBlurEffect::~SsaoBlurEffect()
{
}
#pragma endregion

#pragma region SkyEffect
SkyEffect::SkyEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	SkyTech = mFX->GetTechniqueByName("SkyTech");
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	CubeMap = mFX->GetVariableByName("gCubeMap")->AsShaderResource();
}

SkyEffect::~SkyEffect()
{
}
#pragma endregion

#pragma region DebugTexEffect
DebugTexEffect::DebugTexEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	ViewArgbTech = mFX->GetTechniqueByName("ViewArgbTech");
	ViewRedTech = mFX->GetTechniqueByName("ViewRedTech");
	ViewGreenTech = mFX->GetTechniqueByName("ViewGreenTech");
	ViewBlueTech = mFX->GetTechniqueByName("ViewBlueTech");
	ViewAlphaTech = mFX->GetTechniqueByName("ViewAlphaTech");

	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	Texture = mFX->GetVariableByName("gTexture")->AsShaderResource();
}

DebugTexEffect::~DebugTexEffect()
{

}

void DebugTexEffect::InitInputLayout(ID3D11Device * device)
{
}

void DebugTexEffect::InitInstancingInputLayout(ID3D11Device * device)
{
}

void DebugTexEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{
}

void DebugTexEffect::PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world)
{
}

ID3DX11EffectTechnique * DebugTexEffect::GetTechnique(UINT techType)
{
	return nullptr;
}

void DebugTexEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{
}

void DebugTexEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{
}

#pragma endregion

#pragma region Effects

BasicEffect*           Effects::BasicFX = 0;
NormalMapEffect*       Effects::NormalMapFX = 0;
BuildShadowMapEffect*  Effects::BuildShadowMapFX = 0;
SsaoNormalDepthEffect* Effects::SsaoNormalDepthFX = 0;
SsaoEffect*            Effects::SsaoFX = 0;
SsaoBlurEffect*        Effects::SsaoBlurFX = 0;
SkyEffect*             Effects::SkyFX = 0;
DebugTexEffect*        Effects::DebugTexFX = 0;

void Effects::InitAll(ID3D11Device* device)
{
	BasicFX = new BasicEffect(device, L"FX/Basic.fxo");

	NormalMapFX       = new NormalMapEffect(device, L"FX/NormalMap.fxo");
	//BuildShadowMapFX  = new BuildShadowMapEffect(device, L"FX/BuildShadowMap.fxo");
	//SsaoNormalDepthFX = new SsaoNormalDepthEffect(device, L"FX/SsaoNormalDepth.fxo");
	//SsaoFX            = new SsaoEffect(device, L"FX/Ssao.fxo");
	//SsaoBlurFX        = new SsaoBlurEffect(device, L"FX/SsaoBlur.fxo");
	//SkyFX             = new SkyEffect(device, L"FX/Sky.fxo");
	DebugTexFX = new DebugTexEffect(device, L"FX/DebugTexture.fxo");
}

void Effects::DestroyAll()
{
	SafeDelete(BasicFX);
	SafeDelete(NormalMapFX);
	SafeDelete(BuildShadowMapFX);
	SafeDelete(SsaoNormalDepthFX);
	SafeDelete(SsaoFX);
	SafeDelete(SsaoBlurFX);
	SafeDelete(SkyFX);
	SafeDelete(DebugTexFX);
}

#pragma endregion

bool Effect::IASetting(ID3D11DeviceContext* context, UINT techType)
{
	//instancing으로 렌더링 할 때
	if (techType & TechniqueType::Instancing)
	{
		if (m_instancing_inputLayout == nullptr)
			return false;
		context->IASetInputLayout(m_instancing_inputLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	else
	{
		if (m_inputLayout == nullptr)
			return false;
		context->IASetInputLayout(m_inputLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	}


	return true;
}

bool Effect::OMSetting(ID3D11DeviceContext* context, bool blending)
{
	if (blending)
	{
		if (!m_blendState)
			return false;
		//출력병합기에 혼합 설정
		float blendFactors[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		context->OMSetBlendState(m_blendState, blendFactors, 0xffffffff);
	}
	else
	{
		//기본설정?
		context->OMSetBlendState(0, 0, 0xffffffff);
	}

	return true;
}

TreebilboardEffect::TreebilboardEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{
	Light3Tech = mFX->GetTechniqueByName("Light3");
	Light3TexAlphaClipTech = mFX->GetTechniqueByName("Light3TexAlphaClip");
	Light3TexAlphaClipFogTech = mFX->GetTechniqueByName("Light3TexAlphaClipFog");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();

	DirLights = mFX->GetVariableByName("gDirLights");
	dirLightSize = mFX->GetVariableByName("dirLight_size")->AsScalar();

	pointLights = mFX->GetVariableByName("gPointLights");
	pointLightSize = mFX->GetVariableByName("pointLight_size")->AsScalar();

	spotLights = mFX->GetVariableByName("gSpotLights");
	spotLightSize = mFX->GetVariableByName("spotLight_size")->AsScalar();

	Mat = mFX->GetVariableByName("gMaterial");

	TreeTextureMapArray = mFX->GetVariableByName("gTreeMapArray")->AsShaderResource();

	Init(device);
}

void TreebilboardEffect::InitInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_inputLayout);
	D3DX11_PASS_DESC passDesc;

	Light3TexAlphaClipTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::TreePointSprite, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));
}

void TreebilboardEffect::InitInstancingInputLayout(ID3D11Device * device)
{

}

void TreebilboardEffect::InitBlendState(ID3D11Device * device)
{
	ReleaseCOM(m_blendState);

	D3D11_BLEND_DESC a2CDesc = { 0 };
	a2CDesc.AlphaToCoverageEnable = true;
	a2CDesc.IndependentBlendEnable = false;
	a2CDesc.RenderTarget[0].BlendEnable = false;
	a2CDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	HR(device->CreateBlendState(&a2CDesc, &m_blendState));
}

bool TreebilboardEffect::IASetting(ID3D11DeviceContext * context, UINT techType)
{

	if (m_inputLayout == nullptr)
		return false;
	context->IASetInputLayout(m_inputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	return true;
}

bool TreebilboardEffect::OMSetting(ID3D11DeviceContext * context, bool blending)
{
	context->OMSetBlendState(m_blendState, 0, 0xffffffff);


	return true;
}

void TreebilboardEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{	//쉐이더에 조명설정
	SetDirLights(directL);
	SetPointLights(pointL);
	SetSpotLights(spotL);
	SetEyePosW(camera.GetPosition());

}

void TreebilboardEffect::PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world)
{

	//인스턴스의 세계행렬과 곱해질 시야투영행렬
	SetViewProj(camera->ViewProj());


	//재질 설정
	SetMaterial(material->basicMat);
}

ID3DX11EffectTechnique * TreebilboardEffect::GetTechnique(UINT techType)
{
	switch (techType)
	{
	case TechniqueType::Light:
		return Light3Tech;
	case TechniqueType::Light | TechniqueType::DiffuseMap:
		return Light3TexAlphaClipTech;
	default:
		return Light3TexAlphaClipTech;
	}

	return nullptr;
}

void TreebilboardEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{
	if (diffuseMap == nullptr)
		return;
	SetTreeTextureMapArray(diffuseMap);
}

void TreebilboardEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{
	if (arr == nullptr)
		return;
	SetTreeTextureMapArray(arr);
}

SimpleLineEffect::SimpleLineEffect(ID3D11Device * device, const std::wstring & filename)
	: Effect(device, filename)
{
	Light1Tech = mFX->GetTechniqueByName("Light1");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();

	Init(device);
}

void SimpleLineEffect::InitInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_inputLayout);
	D3DX11_PASS_DESC passDesc;

	Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::PosColor, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));
}

void SimpleLineEffect::InitInstancingInputLayout(ID3D11Device * device)
{
}

void SimpleLineEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{
	//쉐이더에 조명설정
	SetDirLights(directL);
	SetEyePosW(camera.GetPosition());
}

void SimpleLineEffect::PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world)
{
	SetWorld(world);
	//인스턴스의 세계행렬과 곱해질 시야투영행렬
	SetViewProj(camera->ViewProj());

}

ID3DX11EffectTechnique * SimpleLineEffect::GetTechnique(UINT techType)
{
	return Light1Tech;
}

void SimpleLineEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{
}

void SimpleLineEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{
}

void BuildShadowMapBilboardEffect::InitInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_inputLayout);
	D3DX11_PASS_DESC passDesc;

	BuildShadowMapAlphaClipTech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::TreePointSprite, 2, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));
}

ID3DX11EffectTechnique * BuildShadowMapBilboardEffect::GetTechnique(UINT techType)
{
	return BuildShadowMapAlphaClipTech;
}

void BuildShadowMapBilboardEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{
	SetDiffuseMapArray(diffuseMap);
}

void BuildShadowMapBilboardEffect::InitInstancingInputLayout(ID3D11Device * device)
{
}

void BuildShadowMapBilboardEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{
	//인스턴스의 세계행렬과 곱해질 시야투영행렬
	SetViewProj(camera.ViewProj());
	//광원의 위치
	SetEyePosW(camera.GetPosition());

}

void BuildShadowMapBilboardEffect::PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world)
{
	SetWorld(world);

	//물체공간 -> 투영공간 변환행렬
	//XMMATRIX worldViewProj = world * camera->ViewProj();
	//SetWorldViewProj(worldViewProj);

	//비균등 비례로 인한 법선벡터 계산에 쓰이는 행렬
	XMMATRIX worldInvTranspose = MathHelper::InverseTranspose(world);
	SetWorldInvTranspose(worldInvTranspose);

	//텍스쳐 변환 I * S * (R) * T
	XMMATRIX texTransform = XMMatrixIdentity() *
		XMMatrixScaling(material->textureTiling.x,
			material->textureTiling.y, 0.0f) *
		XMMatrixTranslation(material->textureOffset.x,
			material->textureOffset.y, 0.0f);
	SetTexTransform(texTransform);
}

void BuildShadowMapBilboardEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{
}

BuildShadowMapBilboardEffect::BuildShadowMapBilboardEffect(ID3D11Device * device, const std::wstring & filename)
	:Effect(device, filename)
{
	BuildShadowMapTech = mFX->GetTechniqueByName("BuildShadowMapTech");
	BuildShadowMapAlphaClipTech = mFX->GetTechniqueByName("BuildShadowMapAlphaClipTech");

	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	WorldViewProj = mFX->GetVariableByName("gWorldViewProj")->AsMatrix();
	World = mFX->GetVariableByName("gWorld")->AsMatrix();
	WorldInvTranspose = mFX->GetVariableByName("gWorldInvTranspose")->AsMatrix();

	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();

	DiffuseMap = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	DiffuseMapArray = mFX->GetVariableByName("gDiffuseMapArray")->AsShaderResource();

	Init(device);
}

bool BuildShadowMapBilboardEffect::IASetting(ID3D11DeviceContext * context, UINT techType)
{
	if (m_inputLayout == nullptr)
		return false;
	context->IASetInputLayout(m_inputLayout);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}



#pragma region TerrainEffect
TerrainEffect::TerrainEffect(ID3D11Device* device, const std::wstring& filename)
	: Effect(device, filename)
{

	ShadowTransform = mFX->GetVariableByName("gShadowTransform")->AsMatrix();
	ShadowMap = mFX->GetVariableByName("gShadowMap")->AsShaderResource();
	isShadowed = mFX->GetVariableByName("isShadowed")->AsScalar();

	BrushVar = mFX->GetVariableByName("brush");

	DirLights = mFX->GetVariableByName("gDirLights");
	dirLightSize = mFX->GetVariableByName("dirLight_size")->AsScalar();

	pointLights = mFX->GetVariableByName("gPointLights");
	pointLightSize = mFX->GetVariableByName("pointLight_size")->AsScalar();

	spotLights = mFX->GetVariableByName("gSpotLights");
	spotLightSize = mFX->GetVariableByName("spotLight_size")->AsScalar();


	Light1Tech = mFX->GetTechniqueByName("Light1");
	Light2Tech = mFX->GetTechniqueByName("Light2");
	Light3Tech = mFX->GetTechniqueByName("Light3");
	Light1FogTech = mFX->GetTechniqueByName("Light1Fog");
	Light2FogTech = mFX->GetTechniqueByName("Light2Fog");
	Light3FogTech = mFX->GetTechniqueByName("Light3Fog");

	TexTransform = mFX->GetVariableByName("gTexTransform")->AsMatrix();
	ViewProj = mFX->GetVariableByName("gViewProj")->AsMatrix();
	EyePosW = mFX->GetVariableByName("gEyePosW")->AsVector();
	FogColor = mFX->GetVariableByName("gFogColor")->AsVector();
	FogStart = mFX->GetVariableByName("gFogStart")->AsScalar();
	FogRange = mFX->GetVariableByName("gFogRange")->AsScalar();
	DirLights = mFX->GetVariableByName("gDirLights");
	Mat = mFX->GetVariableByName("gMaterial");

	MinDist = mFX->GetVariableByName("gMinDist")->AsScalar();
	MaxDist = mFX->GetVariableByName("gMaxDist")->AsScalar();
	MinTess = mFX->GetVariableByName("gMinTess")->AsScalar();
	MaxTess = mFX->GetVariableByName("gMaxTess")->AsScalar();
	TexelCellSpaceU = mFX->GetVariableByName("gTexelCellSpaceU")->AsScalar();
	TexelCellSpaceV = mFX->GetVariableByName("gTexelCellSpaceV")->AsScalar();
	WorldCellSpace = mFX->GetVariableByName("gWorldCellSpace")->AsScalar();
	WorldFrustumPlanes = mFX->GetVariableByName("gWorldFrustumPlanes")->AsVector();

	LayerMapArray = mFX->GetVariableByName("gLayerMapArray")->AsShaderResource();
	BlendMap = mFX->GetVariableByName("gBlendMap")->AsShaderResource();
	HeightMap = mFX->GetVariableByName("gHeightMap")->AsShaderResource();

	Init(device);
}

TerrainEffect::~TerrainEffect()
{
}
void TerrainEffect::InitInputLayout(ID3D11Device * device)
{
	ReleaseCOM(m_inputLayout);
	D3DX11_PASS_DESC passDesc;

	Light1Tech->GetPassByIndex(0)->GetDesc(&passDesc);
	HR(device->CreateInputLayout(InputLayoutDesc::Terrain, 3, passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize, &m_inputLayout));
}
void TerrainEffect::InitInstancingInputLayout(ID3D11Device * device)
{

}
void TerrainEffect::PerFrameSet(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const Camera & camera)
{
	//쉐이더에 조명설정
	SetDirLights(directL);
	SetPointLights(pointL);
	SetSpotLights(spotL);

	//그림자 맵 설정
	SetShadowMap(g_shadowSRV);

	SetEyePosW(camera.GetPosition());
}
void TerrainEffect::PerObjectSet(GeneralMaterial * material, Camera * camera, CXMMATRIX & world)
{
	//인스턴스의 세계행렬과 곱해질 시야투영행렬
	SetViewProj(camera->ViewProj());

	//텍스쳐 변환 I * S * (R) * T
	XMMATRIX texTransform = XMMatrixIdentity() *
		XMMatrixScaling(material->textureTiling.x,
			material->textureTiling.y, 0.0f) *
		XMMatrixTranslation(material->textureOffset.x,
			material->textureOffset.y, 0.0f);
	SetTexTransform(texTransform);

	//세계공간 -> 광원의 ndc좌표 -> 텍스쳐공간 좌표 변환 행렬
	SetShadowTransform(g_shadowMatrix);

	//재질 설정
	SetMaterial(material->basicMat);
}

ID3DX11EffectTechnique * TerrainEffect::GetTechnique(UINT techType)
{
	if (techType & TechniqueType::Shadowed)
		SetIsShadowed(true);
	else
		SetIsShadowed(false);

	techType = techType & ~TechniqueType::Shadowed;

	switch (techType)
	{

	case TechniqueType::Light:
		return Light3Tech;
	case TechniqueType::Light | TechniqueType::DiffuseMap:
		return Light3Tech;
	case TechniqueType::Light | TechniqueType::Fog:
	case TechniqueType::Light | TechniqueType::DiffuseMap | TechniqueType::Fog:
		return Light3FogTech;
	
	default:
		nullptr;
	}
	return nullptr;
}
void TerrainEffect::SetMaps(ID3D11ShaderResourceView * diffuseMap, ID3D11ShaderResourceView * normalMap, ID3D11ShaderResourceView * specularMap)
{

}
void TerrainEffect::SetMapArray(ID3D11ShaderResourceView * arr)
{

}
bool TerrainEffect::IASetting(ID3D11DeviceContext * context, UINT techType)
{
	////instancing으로 렌더링 할 때
	//if (techType & TechniqueType::Instancing)
	//{
	//	if (m_instancing_inputLayout == nullptr)
	//		return false;
	//	context->IASetInputLayout(m_instancing_inputLayout);
	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//}
	//else
	{
		if (m_inputLayout == nullptr)
			return false;
		context->IASetInputLayout(m_inputLayout);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	}


	return true;
}

#pragma endregion