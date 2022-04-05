#pragma once
#include "Interface.h"
#include "Renderer.h"
#include "Light.h"

#include <vector>
#include <map>
#include <string>
#include <functional>


#define MESHID "MESHRENDERER"
#define SKINNEDMESHID "SKINNEDMESHRENDERER"
#define LIGHTID "LIGHT"

#define MESHRENDERERSIZE 1000
#define SKINNEDMESHRENDERERSIZE 1000
#define LIGHTSIZE 1000
#define COMPONENTSIZE 10000



enum Command
{
	ENABLE,
	DISABLE
};

class ComponentMgr
{
private:
	std::vector<MeshRenderer> meshRenderers;
	std::vector<SkinnedMeshRenderer> skinnedMeshRenderers;
	std::vector<Lighting> lightings;

	std::vector<Component*> components;
	
	//component의 id와 배열 index 매핑
	std::unordered_map<componentID, int> idMap;
		
private:
	//Component를 만들때 사용할 id넘버
	int creatingIdNum;
	//활성화 된 컴포넌트의 개수
	int enableCount_meshRenderer;
	int enableCount_skinnedMeshRenderer;
	int enableCount_lighting;
	
private:
	//Enable, Disable에 따른 해당 Component 컨테이너 원소 위치조정 함수
	template <typename compType>
	Component* SwapEnable(std::vector<compType>& vec, int& enableCount, int idx);
	template <typename compType>
	Component* SwapDisable(std::vector<compType>& vec, int& enableCount, int idx);
	
	//SwapEnable 함수와 SwapDisable 함수 중 하나를 선택
	template <typename compType>
	Component* SelectSwap(std::vector<compType>& vec, int& enableCount, int idx, Command enDisable);
	template <typename compType>
	Component* SelectSwap(std::vector<compType*>& vec, int& enableCount, int idx, Command enDisable);


public:
	ComponentMgr();
	~ComponentMgr();
	static ComponentMgr& Instance();
public:
	//전달받은 Component를 Enable또는 Disable 시키는 함수
	Component* OnOffComponent(Component* component, Command endisable);
	Component* CreateComponent(ComponentType compType);
	
	//ID에서 Component의 타입을 추출
	ComponentType typeOfID(componentID& id);
	//ID로 컴포넌트를 얻어오는 함수
	Component* GetComponent(componentID& id);

	//현재 렌더링가능한 렌더러들을 반환한다.
	const std::vector<Renderer*> GetDrawableRenderers();
	
public:
	void Render(ID3D11DeviceContext* context, Camera* camera);
	void Update();

public:
	const std::vector<Lighting> getLightings() 
	{
		return vector<Lighting>(lightings.begin(), lightings.begin() + enableCount_lighting);
	}
};

template<typename compType>
inline Component* ComponentMgr::SwapEnable(std::vector<compType>& vec, int & enableCount, int idx)
{
	//비활성화 컴포넌트인지 검사
	assert(idx >= enableCount);
		

	//비활성화된 컴포넌트를 제일 앞에 있는 비활성화된 컴포넌트와 바꿈
	std::swap(vec[enableCount], vec[idx]);

	//id와 index를 매핑하는 해쉬맵 업데이트
	idMap[vec[enableCount].id] = enableCount;
	idMap[vec[idx].id] = idx;

	//활성화된 카운트 수 증가
	enableCount++;
	
	

	return &vec[enableCount - 1];
}

template<typename compType>
inline Component* ComponentMgr::SwapDisable(std::vector<compType>& vec, int & enableCount, int idx)
{
	//활성화 된 컴포넌트인지 검사
	assert(idx < enableCount);
	
	//활성화 된 카운트 수 감소
	enableCount--;

	//현재 활성화된 컴포넌트 중 가장 마지막 컴포넌트와 바꿈
	std::swap(vec[enableCount], vec[idx]);
	

	//id와 index를 매핑하는 해쉬맵 업데이트
	idMap[vec[enableCount].id] = enableCount;
	idMap[vec[idx].id] = idx;
	
	return &vec[enableCount];
}

template<typename compType>
inline Component * ComponentMgr::SelectSwap(std::vector<compType>& vec, int & enableCount, int idx, Command enDisable)
{
	if (enDisable == Command::ENABLE)
	{
		//비활성화 컴포넌트인지 검사
		assert(idx >= enableCount);


		//비활성화된 컴포넌트를 제일 앞에 있는 비활성화된 컴포넌트와 바꿈
		std::swap(vec[enableCount], vec[idx]);

		//id와 index를 매핑하는 해쉬맵 업데이트
		idMap[vec[enableCount].id] = enableCount;
		idMap[vec[idx].id] = idx;

		//활성화된 카운트 수 증가
		enableCount++;
				
		vec[enableCount - 1].Enable();
		return &vec[enableCount - 1];		   		
	}
	else if(enDisable == Command::DISABLE)
	{
		//활성화 된 컴포넌트인지 검사
		assert(idx < enableCount);

		//활성화 된 카운트 수 감소
		enableCount--;

		//현재 활성화된 컴포넌트 중 가장 마지막 컴포넌트와 바꿈
		std::swap(vec[enableCount], vec[idx]);


		//id와 index를 매핑하는 해쉬맵 업데이트
		idMap[vec[enableCount].id] = enableCount;
		idMap[vec[idx].id] = idx;

	
	
		vec[enableCount].Disable();
		return &vec[enableCount];
	

		
	}

	return &vec[idx];
}

template<typename compType>
inline Component * ComponentMgr::SelectSwap(std::vector<compType*>& vec, int & enableCount, int idx, Command enDisable)
{
	if (enDisable == Command::ENABLE)
	{
		//비활성화 컴포넌트인지 검사
		assert(idx >= enableCount);


		//비활성화된 컴포넌트를 제일 앞에 있는 비활성화된 컴포넌트와 바꿈
		std::swap(vec[enableCount], vec[idx]);

		//id와 index를 매핑하는 해쉬맵 업데이트
		idMap[vec[enableCount].id] = enableCount;
		idMap[vec[idx].id] = idx;

		//활성화된 카운트 수 증가
		enableCount++;

		vec[enableCount - 1]->Enable();
		return vec[enableCount - 1];
	}
	else if (enDisable == Command::DISABLE)
	{
		//활성화 된 컴포넌트인지 검사
		assert(idx < enableCount);

		//활성화 된 카운트 수 감소
		enableCount--;

		//현재 활성화된 컴포넌트 중 가장 마지막 컴포넌트와 바꿈
		std::swap(vec[enableCount], vec[idx]);


		//id와 index를 매핑하는 해쉬맵 업데이트
		idMap[vec[enableCount].id] = enableCount;
		idMap[vec[idx].id] = idx;



		vec[enableCount]->Disable();
		return vec[enableCount];
	}

	return vec[idx];
}



