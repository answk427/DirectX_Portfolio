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
	
	//component�� id�� �迭 index ����
	std::unordered_map<componentID, int> idMap;
		
private:
	//Component�� ���鶧 ����� id�ѹ�
	int creatingIdNum;
	//Ȱ��ȭ �� ������Ʈ�� ����
	int enableCount_meshRenderer;
	int enableCount_skinnedMeshRenderer;
	int enableCount_lighting;
	
private:
	//Enable, Disable�� ���� �ش� Component �����̳� ���� ��ġ���� �Լ�
	template <typename compType>
	Component* SwapEnable(std::vector<compType>& vec, int& enableCount, int idx);
	template <typename compType>
	Component* SwapDisable(std::vector<compType>& vec, int& enableCount, int idx);
	
	//SwapEnable �Լ��� SwapDisable �Լ� �� �ϳ��� ����
	template <typename compType>
	Component* SelectSwap(std::vector<compType>& vec, int& enableCount, int idx, Command enDisable);
	template <typename compType>
	Component* SelectSwap(std::vector<compType*>& vec, int& enableCount, int idx, Command enDisable);


public:
	ComponentMgr();
	~ComponentMgr();
	static ComponentMgr& Instance();
public:
	//���޹��� Component�� Enable�Ǵ� Disable ��Ű�� �Լ�
	Component* OnOffComponent(Component* component, Command endisable);
	Component* CreateComponent(ComponentType compType);
	
	//ID���� Component�� Ÿ���� ����
	ComponentType typeOfID(componentID& id);
	//ID�� ������Ʈ�� ������ �Լ�
	Component* GetComponent(componentID& id);

	//���� ������������ ���������� ��ȯ�Ѵ�.
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
	//��Ȱ��ȭ ������Ʈ���� �˻�
	assert(idx >= enableCount);
		

	//��Ȱ��ȭ�� ������Ʈ�� ���� �տ� �ִ� ��Ȱ��ȭ�� ������Ʈ�� �ٲ�
	std::swap(vec[enableCount], vec[idx]);

	//id�� index�� �����ϴ� �ؽ��� ������Ʈ
	idMap[vec[enableCount].id] = enableCount;
	idMap[vec[idx].id] = idx;

	//Ȱ��ȭ�� ī��Ʈ �� ����
	enableCount++;
	
	

	return &vec[enableCount - 1];
}

template<typename compType>
inline Component* ComponentMgr::SwapDisable(std::vector<compType>& vec, int & enableCount, int idx)
{
	//Ȱ��ȭ �� ������Ʈ���� �˻�
	assert(idx < enableCount);
	
	//Ȱ��ȭ �� ī��Ʈ �� ����
	enableCount--;

	//���� Ȱ��ȭ�� ������Ʈ �� ���� ������ ������Ʈ�� �ٲ�
	std::swap(vec[enableCount], vec[idx]);
	

	//id�� index�� �����ϴ� �ؽ��� ������Ʈ
	idMap[vec[enableCount].id] = enableCount;
	idMap[vec[idx].id] = idx;
	
	return &vec[enableCount];
}

template<typename compType>
inline Component * ComponentMgr::SelectSwap(std::vector<compType>& vec, int & enableCount, int idx, Command enDisable)
{
	if (enDisable == Command::ENABLE)
	{
		//��Ȱ��ȭ ������Ʈ���� �˻�
		assert(idx >= enableCount);


		//��Ȱ��ȭ�� ������Ʈ�� ���� �տ� �ִ� ��Ȱ��ȭ�� ������Ʈ�� �ٲ�
		std::swap(vec[enableCount], vec[idx]);

		//id�� index�� �����ϴ� �ؽ��� ������Ʈ
		idMap[vec[enableCount].id] = enableCount;
		idMap[vec[idx].id] = idx;

		//Ȱ��ȭ�� ī��Ʈ �� ����
		enableCount++;
				
		vec[enableCount - 1].Enable();
		return &vec[enableCount - 1];		   		
	}
	else if(enDisable == Command::DISABLE)
	{
		//Ȱ��ȭ �� ������Ʈ���� �˻�
		assert(idx < enableCount);

		//Ȱ��ȭ �� ī��Ʈ �� ����
		enableCount--;

		//���� Ȱ��ȭ�� ������Ʈ �� ���� ������ ������Ʈ�� �ٲ�
		std::swap(vec[enableCount], vec[idx]);


		//id�� index�� �����ϴ� �ؽ��� ������Ʈ
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
		//��Ȱ��ȭ ������Ʈ���� �˻�
		assert(idx >= enableCount);


		//��Ȱ��ȭ�� ������Ʈ�� ���� �տ� �ִ� ��Ȱ��ȭ�� ������Ʈ�� �ٲ�
		std::swap(vec[enableCount], vec[idx]);

		//id�� index�� �����ϴ� �ؽ��� ������Ʈ
		idMap[vec[enableCount].id] = enableCount;
		idMap[vec[idx].id] = idx;

		//Ȱ��ȭ�� ī��Ʈ �� ����
		enableCount++;

		vec[enableCount - 1]->Enable();
		return vec[enableCount - 1];
	}
	else if (enDisable == Command::DISABLE)
	{
		//Ȱ��ȭ �� ������Ʈ���� �˻�
		assert(idx < enableCount);

		//Ȱ��ȭ �� ī��Ʈ �� ����
		enableCount--;

		//���� Ȱ��ȭ�� ������Ʈ �� ���� ������ ������Ʈ�� �ٲ�
		std::swap(vec[enableCount], vec[idx]);


		//id�� index�� �����ϴ� �ؽ��� ������Ʈ
		idMap[vec[enableCount].id] = enableCount;
		idMap[vec[idx].id] = idx;



		vec[enableCount]->Disable();
		return vec[enableCount];
	}

	return vec[idx];
}



