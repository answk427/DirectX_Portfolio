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
		//�ν��Ͻ� �� ������Ʈ�� ������Ʈ
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
	//�������
	XMMATRIX world = XMLoadFloat4x4(obj->GetTransform()->GetWorld());
	//local AABB
	const XNA::AxisAlignedBox& objAABB = obj->GetMesh()->GetAABB();
	
	//aabb����->������Ŀ���
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

	//������Ʈ�� ����� ������ ���ԵǴ��� �˻��Ѵ�.
	
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
	//x,y,z������ �������� ���� �� ��
	XMFLOAT3 offset;
	float fStep = parent->m_radius*0.5;
	//�ڽĳ��� ��ȸ
	for (int i = 0; i < 8; ++i)
	{
		OctreeNode* child = parent->m_children[i];
		if (child != nullptr)
		{
			//AABB�� �ڽĿ�����Ʈ�� ������ ������ �˻�
			if (inNode(renderer, child->GetAABB()))
			{
				return BuildOctree(child, renderer, depth + 1);
			}
		}
		else //�ڽĳ�尡 ���� ���
		{
			offset.x = (i & 1) ? -fStep : fStep;
			offset.y = (i & 4) ? -fStep : fStep;
			offset.z = (i & 2) ? -fStep : fStep;
			
			//���� ���� �ڽĳ���� AABB�� ���Ѵ�.
			XNA::AxisAlignedBox nodeAABB;
			const XNA::AxisAlignedBox& parentAABB = parent->GetAABB();
			CreateNodeAABB(&nodeAABB,
				{ parentAABB.Center.x + offset.x , parentAABB.Center.y + offset.y, 
				parentAABB.Center.z + offset.z }, fStep);

			//�ڽĳ�忡 ������Ʈ�� ���� ��� ���ο� �ڽ��� �����.
			if (inNode(renderer, nodeAABB))
			{
				OctreeNode* childNode = new OctreeNode(nodeAABB, fStep);
				parent->m_children[i] = childNode;
				parent->m_children[i]->SetParent(parent);
				//������ ����� AABB�� �������� �߰�
				m_OctreeRenderer->AddBoundingBox(nodeAABB);
				return BuildOctree(childNode, renderer, depth + 1);
			}
		}
	}
	//� �ڽĳ�忡�� ������Ʈ�� ���Ե��� ���ϸ� �����忡 ����.
	parent->AddObject(renderer);

	return parent;
}

OctreeNode * Octree::BuildOctree(Renderer * renderer, int depth)
{
	if (rootNode == nullptr)
	{
		//rootNode�� ������ ����
		XNA::AxisAlignedBox nodeAABB;
		//��Ʈ����� AABB�� �߽ɰ� ���� ��������ŭ�� �Ÿ��� ���������Ѵ�.
		CreateNodeAABB(&nodeAABB, { 0,0,0 }, WIDTH / 2 / OctreeFactor);
		rootNode = new OctreeNode(nodeAABB, WIDTH/2);
		//������ ����� AABB�� �������� �߰�
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
	//��������� ����ü�� ��Ʈ����� ��������
	if (m_frustum->WorldFrustumCulling(&parent->GetAABB()) == 0)
		return;

	//������ �� ������Ʈ�� ������ ��� �߰�
	if (!parent->EmptyObjects())
		m_RenderObjects.push_back(parent);

	//�ڽ� ��带 ��������� Ž��
	for (int i = 0; i < 8; ++i)
	{
		if (parent->m_children[i] != nullptr)
		{
			Travel(parent->m_children[i]);
		}
	}
}



