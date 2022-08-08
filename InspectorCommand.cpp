#include "InspectorCommand.h"

void AddComponentCommand::Execute()
{
	//이미 추가된 Component면 return
	if (gameObj->SearchComponent(componentType))
		return;

	addedComponent = componentMgr.CreateComponent(componentType, gameObj->GetID());
	//Component 생성에 실패한 경우
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

	//tab컨트롤을 업데이트 하기위해 inspector 클래스에 메시지를 보냄.
	WPARAM wparam = (UINT)hInspector;
	SendMessage(*hInspector, WM_COMMAND, wparam, 0);
	
}

void AddComponentCommand::Undo()
{
	if (addedComponent== nullptr)
		return;
	gameObj->DeleteComponent(addedComponent->id);
}
