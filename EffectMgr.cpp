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

	//������ effect�� ã�� �� ���� ��
	else if (myEffect.find(shaderName) == myEffect.end())
	{
		//������ Ÿ���� ������ �⺻���� ����
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
		return myEffect[shaderName]; //�̹� ������������ ��ȯ

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
	//lightings�� ������Ʈ �Ŵ������� �������� lightings �迭
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
	
	//������ ������ nullptr, ������ �迭�� �ּ�
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


