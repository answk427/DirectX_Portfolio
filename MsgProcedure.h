#pragma once
#include <windowsx.h>
#include <Windows.h>
#include <resource.h>
#include <string>
class MsgProcedure
{
protected:
	HINSTANCE m_hInstance;
	
public:
	virtual bool OpenDialog() = 0;
	
public:
	MsgProcedure(HINSTANCE hInstance) :m_hInstance(hInstance){}
	
	bool FileOpenDialog(HWND hwnd, WCHAR fileTitle[], WCHAR filePath[]);
	bool FileSaveDialog(HWND hwnd, WCHAR fileTitle[]);
public:	


	//�޴��������� WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) = 0;
	
	//���� Ű �߻� �޽��� WM_CHAR
	//��ҹ��� ���� ��� wParam : �ƽ�Ű �ڵ� ��
	virtual void CharProc(HWND hDlg, WPARAM wParam) = 0;
		
	virtual void KeyDownProc(WPARAM wParam) = 0;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) = 0;
	
	
};

/*Dialog
- ���̾�α׵� �������̴�.

1. ����� ���̾�α�
- ���̾�αװ� �ֿ켱 ���� ex)�޽����ڽ�
- ����� ���̾�αװ� ������� ������.(�ٸ� â ���� �Ұ�)

2. �𵨸����� ���̾�α�



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


/*
�� ��Ʈ���� ���� ������ �̴�.
��Ʈ���� ������ �ڵ��� ��� �Լ�
HWND GetDlgItem(HWND hDlg, int nIDDlgItem)

��Ʈ�� �޽��� ó��
- ����ڰ� ��Ʈ�ѿ� ���� ������ �ϸ� 
  ���� ��Ʈ���� ������WM_COMMAND �޽��� �߻��� ���� ����
  - LOWORD(wParam) = ���̵� / HIWORD(wParam) = ��Ʈ�� ����, ���� �ڵ�
- �����ڵ�(MSDN ����)
	BN_CLICKED ��ư�� Ŭ���� ���� BN_DBCLICK ��ư�� ����Ŭ���� ����

�� ��Ʈ�� �Ӽ�
 - �⺻ �Ӽ�
	-Caption : ��Ʈ�ѿ� ǥ�õǴ� �ؽ�Ʈ
	-ID : ��Ʈ���� ���̵�
	-Visible : ��Ʈ���� ��� ���θ� ����
*/

/* ����Ʈ ��Ʈ��
 - ����ڷκ��� ������ �Է�
 - ������ �Է�, ����, ������ ���� ��Ʈ��
 - �پ��� �Ӽ� ����

 ���� �б� �Լ�
 UINT GetDlgItemText(HWND hDlg,
	int nIDDlgItem, LPTSTR lpStr,int nMaxCount)
 
 UINT GetDlgItemInt(HWND hDlg, int nIDDlgItem, 
	BOOL *lpTranslated, BOOL bSigned);
	lpTranslated�� ���� null
	bSigned�� true�� ����,��� �� ����.
	bSigned�� false�� ����� ������ 0���� ����
	�ε��Ҽ����� ���ڿ��� �޾Ƽ� ��ȯ�ؾ���. ex) atof

 Edit Control�� ������ ����
 BOOL SetDlgItemText(HWND hDlg, int nIDDlgItem, LPCTSTR lpString)
 BOOL SetDlgItemInt(HWND hDlg, int nIDDlgItem, UINT uValue, BOOL bSigned)

*/

/* �޺��ڽ�
Simple, Drop Down, Drop List ���� ����

 �׸��� �������� ���� ����(�޺��ڽ� ����Ʈ�� �ʹ� ������� ����?)
 - ���ҽ� �信�� �޺��ڽ��� ȭ��ǥ �κ��� Ŭ���ϸ� ���Ʒ��� ũ�⸦ ������ �� ����.

 ��ũ�� �Լ�
 - Windowsx.h ����
 - MSDN���� combo box�� �˻�
 - ��ũ�� �Լ�
 ComboBox_AddString() => �޺��ڽ��� �� �׸��� �߰�
 ComboBox_DeleteString()
 ComboBox_InsertString()
 ComboBox_GetCurSel() => ���õ� �׸��� �ε����� ��ȯ(0���� ����)
  ���� �׸��� ������ CB_ERR��ȯ(-1)
*/

/* ����Ʈ�ڽ�
 �޺��ڽ��� ������
 ListBox_AddString() => ����Ʈ�ڽ��� �� �׸� �߰�
 int ListBox_GetCurSel(HWND hwnd) => ���õ� �׸��� �ε��� ��ȯ
  ���� �׸��� ������ LB_ERR(-1) ��ȯ
*/

/* ����Ʈ��Ʈ�� (����Ʈ�ڽ��� �ٸ�) <- ����Ž���� ���鶧 ����ɵ�?
���� ��� Commctrl.h , Windowsx.h

 �� ����Ʈ�ڽ����� ����
  - ����Ʈ�ڽ��� �����׸��� ����
  - ����Ʈ��Ʈ���� ����Ʈ �ڽ��� ���� �� ���� �� ����.
  - �پ��� �� ��Ÿ�� ����, ��ũ�� �Լ� ����
 �� �佺Ÿ��
  - Icon ��Ÿ��, Small Icon ��Ÿ��, List ��Ÿ��, Report ��Ÿ��
 
 - �÷�(����� �׸�)
 - ���� ������ �̶�� ��.
 - ���� �ִ� ������ SubItem �̶�� ��.

 ���� 1. �÷� ����
 int ListView_InsertColumn(HWND hwnd, int iCol,
	const LPLVCOLUMN pcol)
	hwnd : ������ �ڵ� GetDlgItem() ���
	iCol : �÷� �ε���
	pCol : LVCOLUMN�� �޸� �ּ�

 �� LVCOLUMN ����ü
	{UINT mask = �÷��� ������ �Ӽ��� ����(.fmt, .cx, .pszText, .isublItem)
	OR���� => �Ϲ������� 4�� �� �����
	- LVCF_FMT : FMT�� �÷��� ���� ������ ����
	- LVCF_WIDTH : ���� ����
	- LVCF_TEXT : ���ڿ� ����
	- LVCF_SUBITEM : ���� ������ ����}

	{ fmt : �÷��� ����������� ������ ���� ���� ����
	  ���� ���
	  - LVCFMT_LEFT
	  - LVCFMT_RIGHT
	  - LVCFMT_CENTER
	  * 0 �ε����� �÷��� ����������� ������ ���� ����}
	{ cx : ���� ���� pszText : ���ڿ� ���� iSubItem : ���� ������ �ε���}

	* ������ ���� ��ũ�� �Լ�
	int ListView_GetItemCount(HWND hwnd)
	-������ ���� ����

	int ListView_InsertItem(HWND hwnd, const LPLVITEM pitem)
	�� LPLVITEM ����ü
	UINT mask -> LVIF_TEXT ����
	int iItem; -> �����ϰ��� �ϴ� �ε���
	int iSubItem; -> 0(����)
	LPTSTR pszText; -> ���ڿ� ����

	���� �������� �ִ� �Լ�
	int ListView_SsetItemText(HWND hwnd,
	int i, int iSubItem, LPCTSTR pszText);

	���� ������ ������ ��������
	void ListView_GetItemText(
		HWND hwnd, int iItem,
		int iSubItem,
		LPTSTR pszText,
		int cchTextMax)

	* ������ ���ÿ� ���� �޽��� �˾Ƴ���
	�߻� �޽���
	- WM_NOTIFY
	- lParam
	  ���� �ڵ�(Ŭ������ ����Ŭ������ ��),
	  ��Ʈ�� �ڵ�, ���̵� ����� �޸� �ּ�

	* lParam �ٷ�� ���
	   NMHDR ����ü ���
	   typedef struct tagNMHDR{
			HWND hwndFrom; //������ �ڵ�
			UINT_PTR idFrom; // ��Ʈ�� ���̵�
			UINT id; // �����ڵ�

	* ������ �ε��� �˾Ƴ���
	int ListView_GetNextItem(
		HWND hwnd,
		int iStart, //������ �ε���, -1
		UINT flags // LVNI_SELECTED)

		LVNI_SELECTED -> ���õ� �������� �ε����� ã�ڴ�.
		������ ���� �� : -1 ����

	* ����Ʈ ��Ʈ�� �� ����
	void ListView_SetExtendedListViewStyle(
		HWND hwndLV, DWORD dwExStyle)
	
	dwExStyle ��
	LVS_EX_FULLROWSELECT(���� ��ü ����) |
	LVS_EX_GRIDLINEES(�׸� ����)

	������ ����
	ListView_GetItemText()
	ListView_SetItemText()

	������ ����
	ListView_InsertItemText()
	
	������ ����
	ListView_DeleteItem(HWND hwnd, int iItem)
	ListView_DeleteAllItem(HWND hwnd)
*/

/* ������ �Լ� ����


*/