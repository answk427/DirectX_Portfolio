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

	int m_nOpen; //�����̹��� �ε���
	int m_Closed; //Ŭ�����̹��� �ε���

public:
	HWND m_hDlg; //dialog
	HWND hWnd; //main window
	HWND m_hTab; //�� ��Ʈ�� �ڵ�
	HWND m_hBtn_addComponent; //Add Component ��ư �ڵ�
	HWND m_hCombo; //component ���� combo
public:
	Inspector(HINSTANCE hInstance);

	static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	// MsgProcedure��(��) ���� ��ӵ�
	virtual bool OpenDialog() override;
	bool OpenDialog(HWND hwnd);
	void Init(HWND hWnd_main);
	

public:
	//�޴��������� WM_COMMAND 
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
