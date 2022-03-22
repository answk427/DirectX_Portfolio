#include "TreeBillBoardRenderer.h"

TreeBillBoardRenderer::TreeBillBoardRenderer() : Renderer("TreeBillBoardRenderer", ComponentType::MESHRENDERER)
{
	mesh = new Mesh("TreeBillBoardMesh");
	
	GeneralMaterial treeMaterial;
	
	//�ؽ�ó�迭�� ����ϵ��� �����ؾ���
	treeMaterial.diffuseMapName = L"Textures/tree0.dds";
	materials.push_back(treeMaterial);
}

void TreeBillBoardRenderer::AddTree(ID3D11Device* device, XMFLOAT3 & pos, XMFLOAT2 & size)
{
	//���� ��Ͽ� �߰�
	treeVertices.push_back({ pos,size });
	//���ο� ����������� ���۸� �����.
	mesh->InitVB(device, treeVertices);
	
	//�ε��� �߰�
	mesh->indices.push_back(treeVertices.size() - 1);
	//���ο� �ε���������� ���۸� �����.
	mesh->InitIB(device);
}
