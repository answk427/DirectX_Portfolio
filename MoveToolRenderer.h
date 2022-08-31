#pragma once
#include "Renderer.h"
#include <Camera.h>
#include <GeometryGenerator.h>
#include <GameObject.h>
#include <RayPicking.h>

#define AXISSIZE 0.25
#define AXISLINEWIDTH 0.1
#define FORCEFACTOR 0.3

class MoveToolRenderer 
{
private:
	ID3D11DepthStencilState * pDSState;
	std::vector<XMFLOAT3> vertices;
	std::vector<UINT> indices;
	//y축, x축, z축 순서
	std::vector<InstancingWorldColor> instancingDatas;
	ID3D11Buffer* mVB;
	ID3D11Buffer* mIB;
	ID3D11Buffer* mInstanceBuffer;
	MoveToolEffect* effect;
	//Renderer* m_Renderer;
	GameObject* m_gameObj;
	
	//오브젝트 하위의 모든 AABB중 가장 큰 AABB의 Center
	XMFLOAT3 mAABBCenter;
	float maxLength = 0.0f;

	//한 축의 AABB
	XNA::AxisAlignedBox mAABB;
	
	XMFLOAT2 m_mouseDistance;
private:
	float scaleFactor;
public:
	int m_selectedAxis;

public:
	MoveToolRenderer(ID3D11Device* device);
	~MoveToolRenderer()
	{
		ReleaseCOM(mVB);
		ReleaseCOM(mIB);
		ReleaseCOM(mInstanceBuffer);
	}
public:
	void GetMoveToolAxes(XMVECTOR& localPos, XMMATRIX& world, XMMATRIX& viewProj,
		XMVECTOR& desetNDCpos, XMVECTOR& destXAxisNDC, XMVECTOR& destYAxisNDC, XMVECTOR& destZAxisNDC);
	//void GetMoveToolAxes(XMVECTOR& posW, XMMATRIX& viewProj,
		//XMVECTOR& desetNDCpos, XMVECTOR& destXAxisNDC, XMVECTOR& destYAxisNDC, XMVECTOR& destZAxisNDC);
	void GetMoveToolAxes(XMVECTOR& posW, XMMATRIX& viewProj);
	void GetAxisVertices(XMVECTOR& ndcPos, XMVECTOR& axisNDC, std::vector<XMVECTOR>& destVertices);
	void GetMoveToolVertices(XMFLOAT3& posW, Camera& camera);
	
	void InitInstancingDatas();
	void InstancingUpdate(ID3D11DeviceContext* context);
	void InitIndices();
	
	//y축을 나타내는 막대 정점들 초기화
	void InitCylinderMesh();
	  
	
	void InitVB(ID3D11Device* device);
	void InitIB(ID3D11Device* device);
	void InitInstanceBuffer(ID3D11Device* device);
	void InitDepthStencil(ID3D11Device* device);
	void Draw(ID3D11DeviceContext* context, Camera* camera);

public:
	bool SetObject(GameObject* gameObj);
	void GetMaxAABB(GameObject* gameObj);
	void InitObject();
	void InitAxisAABB(float width, float height);

	void AxisAction(UINT idx, XMMATRIX & viewProj);
	void AxisAction(XMMATRIX & viewProj);
	float CalcForce(XMFLOAT2 & mouseDiff, const XMVECTOR& axisStartH,
		const XMVECTOR& axisEndH);

	void SetMouseDistance(float dx, float dy);
	bool AxisIntersect(D3D11_VIEWPORT* viewPort, Camera* camera,
		float sx, float sy);
};
