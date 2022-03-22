#include "TreeBillBoardRenderer.h"

TreeBillBoardRenderer::TreeBillBoardRenderer() : Renderer("TreeBillBoardRenderer", ComponentType::MESHRENDERER)
{
	mesh = new Mesh("TreeBillBoardMesh");
	
	GeneralMaterial treeMaterial;
	
	//텍스처배열을 사용하도록 수정해야함
	treeMaterial.diffuseMapName = L"Textures/tree0.dds";
	materials.push_back(treeMaterial);
}

void TreeBillBoardRenderer::AddTree(ID3D11Device* device, XMFLOAT3 & pos, XMFLOAT2 & size)
{
	//정점 목록에 추가
	treeVertices.push_back({ pos,size });
	//새로운 정점목록으로 버퍼를 만든다.
	mesh->InitVB(device, treeVertices);
	
	//인덱스 추가
	mesh->indices.push_back(treeVertices.size() - 1);
	//새로운 인덱스목록으로 버퍼를 만든다.
	mesh->InitIB(device);
}
