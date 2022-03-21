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
#include "BoundingBoxRenderer.h"
#include <unordered_map>

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
	BoundingBoxRenderer* m_boundingBoxRenderer;
	//hierarchy에 등록된 오브젝트의 id와 item을 매핑(id를 통해 item을 선택)
	std::unordered_map<gameObjectID, HTREEITEM> Map_idItem;
	
public:
	static bool instantiated;

	int m_nOpen; //오픈이미지 인덱스
	int m_Closed; //클로즈이미지 인덱스

	HWND m_hDlg; //dialog
	HWND m_hwndTV; //tree view
	HWND hWnd; //main window


	HierarchyDialog(HINSTANCE hInstance);

	static INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;

	bool OpenDialog(HWND hwnd);
	void Init(HWND hWnd_main, BoundingBoxRenderer* bbr);
	void treeInit(HWND hDlg);
	void SelectObject(gameObjectID& objID);



public:
	//TreeView 함수
	HTREEITEM TreeViewInsertItem(Object* item, HTREEITEM parent = TVI_ROOT);
	HTREEITEM TreeInsertObject(Object* obj, HTREEITEM parent = TVI_ROOT);

	bool TreeViewDeleteItem(HTREEITEM item);
	bool TreeViewDeleteItem();
	bool TreeImageSet();

	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;
	   	 
	void NotifyProc(LPARAM lParam);

	void WindowSizing(HWND hDlg);

	void SelectItem();

	void Update() { inspector.Update(); }

};
