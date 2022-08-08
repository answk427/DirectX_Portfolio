#include "InspectorCommand.h"

void AddComponentCommand::Execute()
{
	//�̹� �߰��� Component�� return
	if (gameObj->SearchComponent(componentType))
		return;

	addedComponent = componentMgr.CreateComponent(componentType, gameObj->GetID());
	//Component ������ ������ ���
	if (addedComponent == nullptr)
		return;

	switch (componentType)
	{
	case ComponentType::MESHRENDERER:
	case ComponentType::SKINNEDMESHRENDERER:
		((Renderer*)addedComponent)->SetTransform(gameObj->transform.get());
		((Renderer*)addedComponent)->SetNodeHierarchy(gameObj->nodeHierarchy);
		break;
	case ComponentType::LIGHT:
		((Lighting*)addedComponent)->SetTransform(gameObj->transform.get());
	}
	
	gameObj->AddComponent(addedComponent);

	//tab��Ʈ���� ������Ʈ �ϱ����� inspector Ŭ������ �޽����� ����.
	WPARAM wparam = (UINT)hInspector;
	SendMessage(*hInspector, WM_COMMAND, wparam, 0);
	
}

void AddComponentCommand::Undo()
{
	if (addedComponent== nullptr)
		return;
	gameObj->DeleteComponent(addedComponent->id);
}
