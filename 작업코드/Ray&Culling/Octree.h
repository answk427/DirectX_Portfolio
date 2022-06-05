#pragma once
#include <vector>
#include <list>
#include "Renderer.h"
#include <xnacollision.h>
#include "Culling.h"
#include "SimpleLineRenderer.h"

#define WIDTH 1000
#define DEPTH_LIMIT 8
//느슨한 옥트리를 위한 계수
#define OctreeFactor 2

class OctreeNode
{
public:
	OctreeNode(const XNA::AxisAlignedBox& aabb, const float radius) : m_aabb(aabb), m_radius(radius)
	{}
	
	~OctreeNode();
public:
	//옥트리는 8개의 자식노드를 가진다.
	OctreeNode* m_children[8]{ 0 };
	//노드의 반지름
	float m_radius;
private:
	
	//노드의 AABB
	XNA::AxisAlignedBox m_aabb;
	OctreeNode* m_parent;
	//현재 노드에 포함된 object
	std::list<Renderer*> m_objects;
	
	
public:
	const XNA::AxisAlignedBox& GetAABB() const { return m_aabb; }
	void SetParent(OctreeNode* parent) { m_parent = parent; }
	void AddObject(Renderer* obj);
	UINT EmptyObjects() { return m_objects.empty(); }
public:
	std::list<Renderer*>& GetList(){ return m_objects; }
public:
	void Update();
	void Render(ID3D11DeviceContext* context, Camera* camera);
};

class Octree
{
public:
	Octree(Frustum* frustum, SimpleLineRenderer* OctreeRenderer) :
		m_frustum(frustum), rootNode(0), m_OctreeRenderer(OctreeRenderer) {}
private:

	OctreeNode* rootNode;
	Frustum* m_frustum;
	std::vector<OctreeNode*> m_RenderObjects; //이번 프레임에 그릴 노드들
	SimpleLineRenderer* m_OctreeRenderer; //옥트리를 렌더링 할 렌더러
	
public:
	//노드의 AABB를 생성
	void CreateNodeAABB(XNA::AxisAlignedBox* pOut,const XMFLOAT3& pos, const float radius);
	//오브젝트가 해당노드에 완전히 포함되는지 검사하는 함수
	bool inNode(const Renderer* obj, const XNA::AxisAlignedBox& nodeAABB);
	//옥트리 노드를 생성하는 함수
	OctreeNode* BuildOctree(OctreeNode* parent, Renderer* renderer, int depth);
	OctreeNode* BuildOctree(Renderer* renderer, int depth);
	//옥트리를 이용해서 컬링 후 렌더링
	void Render(ID3D11DeviceContext* context);
	//컬링 후 렌더링 할 노드들 탐색
	void Travel(OctreeNode* parent);
};