#pragma once
#include <unordered_map>
#include "Effects.h"
#include "Light.h"
#include <filesystem>

namespace fs = std::filesystem;

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

	//���̴� �̸��� ������ effect�� ����
	std::unordered_map<std::wstring, Effect*> myEffect;
	//���̴� �̸��� effect Type�� ����
	std::unordered_map<std::wstring, EffectType> effectTypes;

	Effect* CreateEffect(const std::wstring& shaderName, EffectType type);
public:
	std::wstring fileDirectory;

	static EffectMgr& Instance();
	EffectMgr();
	~EffectMgr();

	void Init(ID3D11Device* device) { this->device = device; }

	Effect* GetEffect(const std::wstring& shaderName);
	
	
	void SetType(const std::wstring& shaderName, EffectType type);
	void SetEffect(const std::wstring& shaderName, Effect* effect) { myEffect[shaderName] = effect; }

	void SetPerFrame(const std::vector<Lighting>& lightings, const Camera & camera);
	
	
};