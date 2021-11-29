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

#define TAB_TRANSFORM IDD_DIALOG3
#define POSITION_X	IDC_EDIT1
#define POSITION_Y	IDC_EDIT2
#define POSITION_Z	IDC_EDIT3
#define ROTATION_X	IDC_EDIT4
#define ROTATION_Y	IDC_EDIT5
#define ROTATION_Z	IDC_EDIT6
#define SCALE_X		IDC_EDIT7
#define SCALE_Y		IDC_EDIT8
#define SCALE_Z		IDC_EDIT9



INT_PTR CALLBACK TransformProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class TransformDialog : public ComponentDialog
{
private:
	static bool instantiated;
	Transform* m_transform;
private:
	//Edit Box Handles
	HWND m_hEdit_PosX;
	HWND m_hEdit_PosY;
	HWND m_hEdit_PosZ;
	HWND m_hEdit_RotX;
	HWND m_hEdit_RotY;
	HWND m_hEdit_RotZ;
	HWND m_hEdit_ScaleX;
	HWND m_hEdit_ScaleY;
	HWND m_hEdit_ScaleZ;
	
public:
	
public:
	TransformDialog(HINSTANCE hInstance);
public:
	bool OpenDialog(HWND hwnd);
	void Init(HWND hDlg);

private:
	MeshRenderer* m_MeshRenderer;

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
	virtual bool SetObject(GameObject * obj) override;

	virtual bool UpdateView() override;

};


