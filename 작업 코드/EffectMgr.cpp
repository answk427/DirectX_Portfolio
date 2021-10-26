#include "EffectMgr.h"

bool EffectMgr::instantiated = false;

EffectMgr::EffectMgr()
{
	assert(!instantiated);
	instantiated = true;
}

EffectMgr::EffectMgr(ID3D11Device * device)
{
	this->device = device;
}

EffectMgr::~EffectMgr()
{
	for (auto it = myEffect.begin(); it != myEffect.end(); ++it)
		delete it->second;

	myEffect.clear();
	instantiated = false;
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

void EffectMgr::SetPerFrame(DirectionalLight * directL, PointLight * pointL, SpotLight * spotL, const XMFLOAT3 & eyePosW)
{
	for (auto effect : myEffect)
	{
		effect.second->PerFrameSet(directL, pointL, spotL, eyePosW);
	}
}


