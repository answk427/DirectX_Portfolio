#pragma once
#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>
#include "MsgProcedure.h"
#include "GameObject.h"
#include "DialogSizeDefine.h"

#pragma comment(lib,"comctl32.lib")


class HierarchyDialog : public MsgProcedure
{
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
	void TreeInsertObject(Object* obj, HTREEITEM parent = TVI_ROOT);
	bool TreeImageSet();

	//�޴��������� WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	   	 
	void NotifyProc(LPARAM lParam);

	void WindowSizing(HWND hDlg);



};
