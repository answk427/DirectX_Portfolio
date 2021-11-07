#pragma once
#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>
#include "MsgProcedure.h"
#include "GameObject.h"

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
	HTREEITEM TreeViewInsertItem(LPWSTR itemName, HTREEITEM parent = TVI_ROOT);
	void TreeInsertObject(Object* obj, HTREEITEM parent = TVI_ROOT);

	//�޴��������� WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;
	
	virtual void KeyDownProc(WPARAM wParam) override;
	
	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;
		   
	virtual void CharProc(HWND hDlg, WPARAM wParam) override;

	

	bool TreeImageSet();

	void NotifyProc(LPARAM lParam);
	
	void testFunc();
	

	
};

/*Dialog
- ���̾�α׵� �������̴�.

1. ����� ���̾�α�
- ���̾�αװ� �ֿ켱 ���� ex)�޽����ڽ�
- ����� ���̾�αװ� ������� ������.(�ٸ� â ���� �Ұ�)
- DialogBox()�� ���� EndDialog()�� ����

2. �𵨸����� ���̾�α�
- CreateDialog()�� ����
- DestroyWindow()�� ����


�� ���̾�α� �����Լ�
- DialogBox(HINSTANCE hInstance,
	LPCTSTR lpTemplate, <- Form�� ����
	HWND hWndParent, <- �޴��� ���ؼ� Dialog�� ������ �޴��� ���� �����찡 �θ�
	DLGPROC lpDialogFunc); <- ���ν���

- makeintresource() -> lpTemplate�� ����
- WndProc���� ó������ ���� �޽����� DefWindowProc�� ó��
	- DialogProc�� TRUE �Ǵ� FALSE�� ��ȯ
- SetWindowText() �� ���̾�α��� Ÿ��Ʋ ����

**********************
���Ͽ��� ����� ���̾�α�
#include <commdlg.h>
BOOL GetOpenFileName(LOPENFILENAME lpofn);
- OPENFILENAME ��� ����
 - lpstrFilter
   �������� ���� ex) "��Ʈ��(*.bmp)\0*.bmp\0
 - nFilterIndex
	������ ���� �ε���, 1���� ����
 - lpstrFileTitle
	���ϸ��� ������ �迭 ����
	nMaxFileTitle�� ���� ����
	static ���� �Ǵ� ���� ���� ����(�ʼ�, ���÷��ϸ� ����)
 - lpstrFile
	'��ü ���'�� ������ �迭 ����
	nMaxFile�� ���� ����
	static ���� �Ǵ� ���� ���� ����
 - lpstrTitle
	���̾�α� Ÿ��Ʋ ���ڿ� ���

*/


/* ���̾�α� ���ν����� ����

// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam)); //2��° �μ��� ���̾�α׺��� ���ϰ��� �ް� ������ ����. ���� 0���� ���
			return (INT_PTR)TRUE;
		}
		break;
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	
	}
	return (INT_PTR)FALSE;
}
*/