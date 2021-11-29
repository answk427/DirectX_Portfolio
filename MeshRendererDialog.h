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
	HWND m_hList; //list view �ڵ�
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
	//�޴��������� WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	
	void NotifyProc(HWND hDlg, LPARAM lParam);

	VOID WINAPI OnChildDialogInit(HWND hwndDlg);

private:
	// MsgProcedure��(��) ���� ��ӵ�
	virtual bool OpenDialog() override;//Tab�� �ڽ� Dialog�� �����ؾ� �ϹǷ� ������� �ʴ� �Լ�

public:
	// ComponentDialog��(��) ���� ��ӵ�
	virtual bool UpdateView() override;


	// ComponentDialog��(��) ���� ��ӵ�
	virtual bool SetObject(GameObject * obj) override;

};


