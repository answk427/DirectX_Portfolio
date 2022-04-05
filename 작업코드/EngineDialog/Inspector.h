#pragma once
#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>


#include "MsgProcedure.h"
#include "GameObject.h"
#include "DialogStructureEX.h"
#include "DialogSizeDefine.h"
#include "MeshRendererDialog.h"
#include "TransformDialog.h"
#include "LightDialog.h"

#include "InspectorCommand.h"

#pragma comment(lib,"comctl32.lib")

#define TAB_TRANSFORM IDD_DIALOG3
#define TAB_TERRAIN	IDD_DIALOG5
#define INSPECTOR IDD_DIALOG6
#define TABCONTROLID IDC_TAB1

#define ADDCOMPONENTBTN IDC_BUTTON1
#define COMPONENTCOMBO IDC_COMBO2



class Inspector : public MsgProcedure
{
private:
	DLGHDR *m_pHdr;
	GameObject* m_currObject;
private:
	MeshRendererDialog m_MeshRendererDialog;
	TransformDialog m_TransformDialog;
	LightDialog m_LightDialog;

	std::vector<ComponentDialog*> dialogs;

public:
	static bool instantiated;

	int m_nOpen; //오픈이미지 인덱스
	int m_Closed; //클로즈이미지 인덱스

public:
	HWND m_hDlg; //dialog
	HWND hWnd; //main window
	HWND m_hTab; //탭 컨트롤 핸들
	HWND m_hBtn_addComponent; //Add Component 버튼 핸들
	HWND m_hCombo; //component 선택 combo
public:
	Inspector(HINSTANCE hInstance);

	static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;
	bool OpenDialog(HWND hwnd);
	void Init(HWND hWnd_main);
	

public:
	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	   	 

	void NotifyProc(HWND hDlg,LPARAM lParam);

	void WindowSizing(HWND hwndDlg);

public:
	// Handles the WM_INITDIALOG message for a dialog box that contains 
//   a tab control used to select among three child dialog boxes.
// Returns a result code.
// hwndDlg - handle of the dialog box.
// 
	HRESULT OnTabbedDialogInit(HWND hwndDlg);

	// Loads and locks a dialog box template resource. 
	// Returns the address of the locked dialog box template resource. 
	// lpszResName - name of the resource. 
	//
	DLGTEMPLATEEX* DoLockDlgRes(LPCTSTR lpszResName);

	// Processes the TCN_SELCHANGE notification. 
// hwndDlg - handle to the parent dialog box. 
//
	VOID OnSelChanged(HWND hwndDlg);
	
	// Positions the child dialog box to occupy the display area of the 
//   tab control. 
// hwndDlg - handle of the dialog box.
//
private:
	void AddTab(HWND hTab);
public:
	void SetObject(GameObject* obj);
	bool AddComponent(ComponentType componentTyp);
	void Update();
	
	
	

};
