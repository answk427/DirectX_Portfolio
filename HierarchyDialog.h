#pragma once
#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>
#include "MsgProcedure.h"
#include "GameObject.h"
#include "DialogSizeDefine.h"
#include "Inspector.h"
#include "ObjectMgr.h"

#pragma comment(lib,"comctl32.lib")

LRESULT CALLBACK myListBoxSubclassProc(HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam,
	UINT_PTR uIdSubclass,
	DWORD_PTR dwRefData);

class HierarchyDialog : public MsgProcedure
{
private:
	Inspector inspector;
public:
	static bool instantiated;

	int m_nOpen; //�����̹��� �ε���
	int m_Closed; //Ŭ�����̹��� �ε���

	HWND m_hDlg; //dialog
	HWND m_hwndTV; //tree view
	HWND hWnd; //main window


	HierarchyDialog(HINSTANCE hInstance);

	static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	// MsgProcedure��(��) ���� ��ӵ�
	virtual bool OpenDialog() override;

	bool OpenDialog(HWND hwnd);
	void Init(HWND hWnd_main);
	void treeInit(HWND hDlg);




public:
	//TreeView �Լ�
	HTREEITEM TreeViewInsertItem(Object* item, HTREEITEM parent = TVI_ROOT);
	HTREEITEM TreeInsertObject(Object* obj, HTREEITEM parent = TVI_ROOT);

	bool TreeViewDeleteItem(HTREEITEM item);
	bool TreeViewDeleteItem();


	bool TreeImageSet();

	//�޴��������� WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	   	 
	void NotifyProc(LPARAM lParam);

	void WindowSizing(HWND hDlg);

	void SelectItem();

	void Update() { inspector.Update(); }

};
