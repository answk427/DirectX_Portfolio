#pragma once
#include <vector>
#include <list>
#include "Renderer.h"
#include <xnacollision.h>
#include "Culling.h"
#include "SimpleLineRenderer.h"

#define WIDTH 1000
#define DEPTH_LIMIT 8
//������ ��Ʈ���� ���� ���
#define OctreeFactor 2

class OctreeNode
{
public:
	OctreeNode(const XNA::AxisAlignedBox& aabb, const float radius) : m_aabb(aabb), m_radius(radius)
	{}
	
	~OctreeNode();
public:
	//��Ʈ���� 8���� �ڽĳ�带 ������.
	OctreeNode* m_children[8]{ 0 };
	//����� ������
	float m_radius;
private:
	
	//����� AABB
	XNA::AxisAlignedBox m_aabb;
	OctreeNode* m_parent;
	//���� ��忡 ���Ե� object
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
	std::vector<OctreeNode*> m_RenderObjects; //�̹� �����ӿ� �׸� ����
	SimpleLineRenderer* m_OctreeRenderer; //��Ʈ���� ������ �� ������
	
public:
	//����� AABB�� ����
	void CreateNodeAABB(XNA::AxisAlignedBox* pOut,const XMFLOAT3& pos, const float radius);
	//������Ʈ�� �ش��忡 ������ ���ԵǴ��� �˻��ϴ� �Լ�
	bool inNode(const Renderer* obj, const XNA::AxisAlignedBox& nodeAABB);
	//��Ʈ�� ��带 �����ϴ� �Լ�
	OctreeNode* BuildOctree(OctreeNode* parent, Renderer* renderer, int depth);
	OctreeNode* BuildOctree(Renderer* renderer, int depth);
	//��Ʈ���� �̿��ؼ� �ø� �� ������
	void Render(ID3D11DeviceContext* context);
	//�ø� �� ������ �� ���� Ž��
	void Travel(OctreeNode* parent);
};