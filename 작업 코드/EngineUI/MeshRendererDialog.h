#pragma once

#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>


#include "ComponentDialog.h"
#include "GameObject.h"
#include "DialogStructureEX.h"
#include "DialogSizeDefine.h"
#include "Renderer.h"

#include "MeshRendererCommand.h"



#define TAB_MESHRENDERER IDD_DIALOG4

#define DIFFUSEMAPCONTROL IDC_EDIT1
#define NORMALMAPCONTROL IDC_EDIT12

#define DIFFUSEMAPBUTTON IDC_BUTTON1
#define NORMALMAPBUTTON IDC_BUTTON3

INT_PTR CALLBACK MeshRendererProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class MeshRendererDialog : public ComponentDialog
{
public:
	static bool instantiated;
public:
	HWND m_hList; //list view 핸들
public:
	MeshRendererDialog(HINSTANCE hInstance);	
public:
	bool OpenDialog(HWND hwnd);
	void Init(HWND hDlg);

private:
	MeshRenderer* m_MeshRenderer;
	std::vector<GeneralMaterial>* materials;
	const Mesh* mesh;

public:
	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	
	void NotifyProc(HWND hDlg, LPARAM lParam);

	VOID WINAPI OnChildDialogInit(HWND hwndDlg);

private:
	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;//Tab의 자식 Dialog로 생성해야 하므로 사용하지 않는 함수

public:
	// ComponentDialog을(를) 통해 상속됨
	virtual bool UpdateView() override;


	// ComponentDialog을(를) 통해 상속됨
	virtual bool SetObject(GameObject * obj) override;

};


