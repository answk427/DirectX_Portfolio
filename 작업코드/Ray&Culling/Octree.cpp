#include "Octree.h"

OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 8; ++i)
	{
		if (!m_children[i])
			delete m_children[i];
	}
}

void OctreeNode::AddObject(Renderer * obj)
{
	for (auto elem : m_objects)
	{
		if (elem->id == obj->id)
			return;
	}
	m_objects.push_back(obj);
}

void OctreeNode::Update()
{
	for (auto renderer : m_objects)
	{
		//인스턴싱 할 오브젝트를 업데이트
		if (renderer->GetInstancing())
		{
			renderer->InstancingUpdate();
		}
	}
}

void OctreeNode::Render(ID3D11DeviceContext* context, Camera* camera)
{
	for (auto renderer : m_objects)
	{
		renderer->Draw(context, camera);
	}
}

void Octree::CreateNodeAABB(XNA::AxisAlignedBox * pOut, const XMFLOAT3 & pos, const float radius)
{
	pOut->Center = pos;
	pOut->Extents = { OctreeFactor * radius, OctreeFactor * radius, OctreeFactor * radius };
}


bool Octree::inNode(const Renderer* obj, const XNA::AxisAlignedBox & nodeAABB)
{
	//세계행렬
	XMMATRIX world = XMLoadFloat4x4(obj->GetTransform()->GetWorld());
	//local AABB
	const XNA::AxisAlignedBox& objAABB = obj->GetMesh()->GetAABB();
	
	//aabb원점->세계행렬원점
	XMVECTOR CenterObj = XMVector3TransformCoord(XMLoadFloat3(&objAABB.Center), world);
	//XMVECTOR CenterObj = XMLoadFloat3(&objAABB.Center);
	XMVECTOR ExtentsObj = XMLoadFloat3(&objAABB.Extents);

	//XMVECTOR CenterNode = XMVector3TransformCoord(XMLoadFloat3(&nodeAABB.Center), worldInv);
	XMVECTOR CenterNode = XMLoadFloat3(&nodeAABB.Center);
	XMVECTOR ExtentsNode = XMLoadFloat3(&nodeAABB.Extents);

	XMVECTOR MinObj = CenterObj - ExtentsObj;
	XMVECTOR MaxObj = CenterObj + ExtentsObj;

	XMVECTOR MinNode = CenterNode - ExtentsNode;
	XMVECTOR MaxNode = CenterNode + ExtentsNode;

	XMFLOAT3 MinObjF, MaxObjF, MinNodeF, MaxNodeF;

	//오브젝트가 노드의 완전히 포함되는지 검사한다.
	
	return XMVector3LessOrEqual(MinNode, MinObj) && XMVector3LessOrEqual(MaxObj, MaxNode);
}

OctreeNode * Octree::BuildOctree(OctreeNode* parent, Renderer * renderer, int depth)
{
	if (depth > DEPTH_LIMIT)
	{
		parent->AddObject(renderer);
		return parent;
	}

	
	XNA::AxisAlignedBox objAABB = renderer->GetMesh()->GetAABB();
	//x,y,z축으로 원점에서 더해 줄 값
	XMFLOAT3 offset;
	float fStep = parent->m_radius*0.5;
	//자식노드들 순회
	for (int i = 0; i < 8; ++i)
	{
		OctreeNode* child = parent->m_children[i];
		if (child != nullptr)
		{
			//AABB가 자식오브젝트에 완전히 들어가는지 검사
			if (inNode(renderer, child->GetAABB()))
			{
				return BuildOctree(child, renderer, depth + 1);
			}
		}
		else //자식노드가 없는 경우
		{
			offset.x = (i & 1) ? -fStep : fStep;
			offset.y = (i & 4) ? -fStep : fStep;
			offset.z = (i & 2) ? -fStep : fStep;
			
			//새로 만들 자식노드의 AABB를 구한다.
			XNA::AxisAlignedBox nodeAABB;
			const XNA::AxisAlignedBox& parentAABB = parent->GetAABB();
			CreateNodeAABB(&nodeAABB,
				{ parentAABB.Center.x + offset.x , parentAABB.Center.y + offset.y, 
				parentAABB.Center.z + offset.z }, fStep);

			//자식노드에 오브젝트가 들어가는 경우 새로운 자식을 만든다.
			if (inNode(renderer, nodeAABB))
			{
				OctreeNode* childNode = new OctreeNode(nodeAABB, fStep);
				parent->m_children[i] = childNode;
				parent->m_children[i]->SetParent(parent);
				//생성한 노드의 AABB를 렌더러에 추가
				m_OctreeRenderer->AddBoundingBox(nodeAABB);
				return BuildOctree(childNode, renderer, depth + 1);
			}
		}
	}
	//어떤 자식노드에도 오브젝트가 포함되지 못하면 현재노드에 포함.
	parent->AddObject(renderer);

	return parent;
}

OctreeNode * Octree::BuildOctree(Renderer * renderer, int depth)
{
	if (rootNode == nullptr)
	{
		//rootNode가 없으면 만듬
		XNA::AxisAlignedBox nodeAABB;
		//루트노드의 AABB는 중심과 면이 반지름만큼의 거리를 가지도록한다.
		CreateNodeAABB(&nodeAABB, { 0,0,0 }, WIDTH / 2 / OctreeFactor);
		rootNode = new OctreeNode(nodeAABB, WIDTH/2);
		//생성한 노드의 AABB를 렌더러에 추가
		m_OctreeRenderer->AddBoundingBox(nodeAABB);
	}
	return BuildOctree(rootNode, renderer, depth+1);
}

void Octree::Render(ID3D11DeviceContext * context)
{
	if (rootNode == nullptr)
		return;
	Travel(rootNode);

	for (auto renderer : m_RenderObjects)
	{
		renderer->Render(context, m_frustum->m_camera);
	}
	m_RenderObjects.clear();
}

void Octree::Travel(OctreeNode * parent)
{
	//세계공간의 절두체와 옥트리노드 교차판정
	if (m_frustum->WorldFrustumCulling(&parent->GetAABB()) == 0)
		return;

	//렌더링 할 오브젝트가 있으면 노드 추가
	if (!parent->EmptyObjects())
		m_RenderObjects.push_back(parent);

	//자식 노드를 재귀적으로 탐색
	for (int i = 0; i < 8; ++i)
	{
		if (parent->m_children[i] != nullptr)
		{
			Travel(parent->m_children[i]);
		}
	}
}



