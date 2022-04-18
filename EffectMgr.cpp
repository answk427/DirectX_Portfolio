#include "EffectMgr.h"



EffectMgr & EffectMgr::Instance()
{
	static EffectMgr* instance = new EffectMgr();
	return *instance;
}

EffectMgr::EffectMgr()
{
	
}

EffectMgr::~EffectMgr()
{
	for (auto it = myEffect.begin(); it != myEffect.end(); ++it)
		delete it->second;

	myEffect.clear();	
}

Effect* EffectMgr::GetEffect(const std::wstring& shaderName)
{
	if (shaderName.empty())
		return nullptr;

	//생성된 effect를 찾을 수 없을 때
	else if (myEffect.find(shaderName) == myEffect.end())
	{
		//지정된 타입이 없으면 기본으로 생성
		if (effectTypes.find(shaderName) == effectTypes.end())
			return CreateEffect(shaderName, EffectType::BasicEffectType);
		else
			return CreateEffect(shaderName, effectTypes[shaderName]);
	}
	else
		return myEffect[shaderName];
}


Effect * EffectMgr::CreateEffect(const std::wstring& shaderName, EffectType type)
{
	if (shaderName.empty())
		return nullptr;
	else if (myEffect.find(shaderName) != myEffect.end())
		return myEffect[shaderName]; //이미 생성되있으면 반환

	Effect* effect=0;

	switch (type)
	{
		case EffectType::BasicEffectType:
			effect = new BasicEffect(device, shaderName);
			myEffect[shaderName] = effect;
			break;
		case EffectType::BillBoardEffectType:
			effect = new TreebilboardEffect(device, shaderName);
			myEffect[shaderName] = effect;
			break;
		case EffectType::BuildShadowMap:
			effect = new BuildShadowMapEffect(device, shaderName);
			myEffect[shaderName] = effect;
			break;
	}

	return effect;
}

void EffectMgr::SetType(const std::wstring & shaderName, EffectType type)
{
	if (effectTypes.find(shaderName) != effectTypes.end())
		return;
	else
		effectTypes[shaderName] = type;

	return;
}

void EffectMgr::SetPerFrame(const std::vector<Lighting>& lightings, const Camera & camera)
{
	//lightings는 컴포넌트 매니저에서 관리중인 lightings 배열
	std::vector<DirectionalLight> dir;
	std::vector<PointLight> point;
	std::vector<SpotLight> spot;

	
	for (auto& lighting : lightings)
	{
		switch (lighting.GetLightType())
		{
		case LightType::DIRECTIONAL:
			dir.push_back(lighting.GetDirLight());
			break;
		case LightType::POINTLIGHT:
			point.push_back(lighting.GetPointLight());
			break;
		case LightType::SPOTLIGHT:
			spot.push_back(lighting.GetSpotLight());
			break;
		default:
			break;
		}
	}
	
	//조명이 없으면 nullptr, 있으면 배열의 주소
	DirectionalLight* dirPt = nullptr;
	PointLight* pointPt = nullptr;
	SpotLight* spotPt = nullptr;

	if (!dir.empty())
		dirPt = &dir[0];
	if (!point.empty())
		pointPt = &point[0];
	if (!spot.empty())
		spotPt = &spot[0];

	for (auto effect : myEffect)
	{
		effect.second->PerFrameSet(dirPt, pointPt, spotPt, camera);
	}
}


