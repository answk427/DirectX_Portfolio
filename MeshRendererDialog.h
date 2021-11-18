#pragma once

#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>


#include "MsgProcedure.h"
#include "GameObject.h"
#include "DialogStructureEX.h"
#include "DialogSizeDefine.h"
#include "Renderer.h"

#define TAB_MESHRENDERER IDD_DIALOG4

INT_PTR CALLBACK MeshRendererProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class MeshRendererDialog : MsgProcedure
{
public:
	
	static bool instantiated;

	int m_nOpen; //오픈이미지 인덱스
	int m_Closed; //클로즈이미지 인덱스

	HWND m_hDlg; //dialog
	HWND m_hwndParent; //main window

	MeshRendererDialog(HINSTANCE hInstance);
		
	bool OpenDialog(HWND hwnd);
	


public:
	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	
	void NotifyProc(HWND hDlg, LPARAM lParam);

	VOID WINAPI OnChildDialogInit(HWND hwndDlg);

	void UpdateView(MeshRenderer* meshRenderer);

private:
	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;//Tab의 자식 Dialog로 생성해야 하므로 사용하지 않는 함수
	
private:
	MeshRenderer* m_MeshRenderer;

};


