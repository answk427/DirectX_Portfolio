#pragma once
#include <unordered_map>
#include "Effects.h"
#include "Light.h"

enum EffectType
{
	BasicEffectType,
	BillBoardEffectType,
	BuildShadowMap,
	Terrain
};

class EffectMgr
{
private:
	EffectMgr(const EffectMgr& effect);
	//EffectMgr& operator=(const EffectMgr& effect);
private:
	ID3D11Device* device;

	//쉐이더 이름과 생성한 effect를 매핑
	std::unordered_map<std::wstring, Effect*> myEffect;
	//쉐이더 이름과 effect Type을 매핑
	std::unordered_map<std::wstring, EffectType> effectTypes;

	
public:
	static EffectMgr& Instance();
	EffectMgr();
	~EffectMgr();

	void Init(ID3D11Device* device) { this->device = device; }

	Effect* GetEffect(const std::wstring& shaderName);
	Effect* CreateEffect(const std::wstring& shaderName, EffectType type);
	
	void SetType(const std::wstring& shaderName, EffectType type);
	void SetEffect(const std::wstring& shaderName, Effect* effect) { myEffect[shaderName] = effect; }

	void SetPerFrame(const std::vector<Lighting>& lightings, const Camera & camera);
	
	
};