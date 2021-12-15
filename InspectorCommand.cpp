#include "InspectorCommand.h"

void AddComponentCommand::Execute()
{
	//�̹� �߰��� Component�� return
	if (gameObj->SearchComponent(componentType))
		return;

	addedComponent = componentMgr.CreateComponent(componentType);
	//Component ������ ������ ���
	if (addedComponent == nullptr)
		return;
	
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
