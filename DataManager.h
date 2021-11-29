#pragma once
#include "EffectMgr.h"

//파일 저장, 불러오기 클래스
class DataManager
{
private:
	EffectMgr& effectMgr;
public:
	DataManager():effectMgr(EffectMgr::Instance()){}
	~DataManager() {}

public:
	static DataManager& Instance()
	{
		static DataManager* instance = new DataManager();
		return *instance;
	}

public:
	bool LoadEffectData() 
	{
		if (effectMgr.CreateEffect(L"FX/Basic.fxo", EffectType::BasicEffectType) == nullptr)
			return false;
		else
			return true;
	}
};
