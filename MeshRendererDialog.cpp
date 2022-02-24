#include "MeshRendererDialog.h"


extern MeshRendererDialog* g_MeshRendererDialog = 0;
bool MeshRendererDialog::instantiated = false;

//-------------------*ChildProc Test *-----------------------
VOID WINAPI MeshRendererDialog::OnChildDialogInit(HWND hwndDlg)
{
	//부모 윈도우의 위치,크기
	HWND hwndParent = GetParent(hwndDlg);
	RECT parentWndRect;
	GetWindowRect(hwndParent, &parentWndRect);
	
	
	float parentWidth = parentWndRect.right - parentWndRect.left;
	float parentHeight = (parentWndRect.bottom - parentWndRect.top);
	SetWindowPos(hwndDlg, HWND_TOP, parentWndRect.left,
		parentWndRect.top + parentHeight * 0.03,//-2,
		parentWidth,
		parentHeight*0.9,
		SWP_SHOWWINDOW);

	return;
}


INT_PTR CALLBACK MeshRendererProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_MeshRendererDialog->OnChildDialogInit(hDlg);
		g_MeshRendererDialog->Init(hDlg);

		
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		g_MeshRendererDialog->MenuProc(hDlg, wParam);
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:

		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:

		return (INT_PTR)TRUE;
	case WM_NOTIFY: //리스트 컨트롤 클릭했을 때 발생
		g_MeshRendererDialog->NotifyProc(hDlg, lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{

		return (INT_PTR)TRUE;
	}

	}
	return (INT_PTR)FALSE;
}
//-------------------*ChildProc Test *-----------------------



bool MeshRendererDialog::OpenDialog()
{
	return false;
}

bool MeshRendererDialog::SetObject(GameObject* obj)
{
	MeshRenderer* meshRenderer = obj->GetComponent<MeshRenderer>();
	m_MeshRenderer = meshRenderer;
	if (!m_MeshRenderer)
		return false;

	materials = &(m_MeshRenderer->GetMaterials());
	mesh = m_MeshRenderer->GetMesh();


	

}

bool MeshRendererDialog::UpdateView()
{	
	if (m_MeshRenderer == nullptr)
		return false;

	
	//ASCII -> UNICODE 변환
	USES_CONVERSION;

	if (mesh != nullptr)
	{
		WCHAR currentName[100] = L"";
		Edit_GetText(GetDlgItem(m_hDlg, IDC_EDIT11), currentName, 100);
		
		LPCWSTR meshName = A2W(mesh->id.c_str());

		//현재 mesh와 editText의 이름이 다르면
		if (wcscmp(currentName, meshName) != 0)
		{
			//mesh editText에 이름 설정
			Edit_SetText(GetDlgItem(m_hDlg, IDC_EDIT11), meshName);
		}
	}
		

	return true;

}


bool MeshRendererDialog::OpenDialog(HWND hwnd)
{

	m_hwndParent = hwnd;

	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(TAB_MESHRENDERER), hwnd, MeshRendererProc);
		
		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void MeshRendererDialog::Init(HWND hDlg)
{
	m_hDlg = hDlg;
	m_hList = GetDlgItem(hDlg, MATERIALLIST);
	
	//material 텍스쳐행렬 수정 에디트박스 핸들
	m_hDiffuseTileX = GetDlgItem(hDlg, DIFFUSETILEX_EDIT);
	m_hDiffuseTileY = GetDlgItem(hDlg, DIFFUSETILEY_EDIT);
	m_hDiffuseOffsetX = GetDlgItem(hDlg, DIFFUSEOFFSETX_EDIT);
	m_hDiffuseOffsetY = GetDlgItem(hDlg, DIFFUSEOFFSETY_EDIT);

	m_hNormalTileX = GetDlgItem(hDlg, NORMALTILEX_EDIT);
	m_hNormalTileY= GetDlgItem(hDlg, NORMALTILEY_EDIT);
	m_hNormalOffsetX = GetDlgItem(hDlg, NORMALOFFSETX_EDIT);
	m_hNormalOffsetY = GetDlgItem(hDlg, NORMALOFFSETY_EDIT);

	//BOUNDING BOX 수정 에디트박스 핸들
	m_BoundMinX = GetDlgItem(hDlg, BOUNDMINX_EDIT);
	m_BoundMinY = GetDlgItem(hDlg, BOUNDMINY_EDIT);
	m_BoundMinZ = GetDlgItem(hDlg, BOUNDMINZ_EDIT);

	m_BoundMaxX = GetDlgItem(hDlg, BOUNDMAXX_EDIT);
	m_BoundMaxY = GetDlgItem(hDlg, BOUNDMAXY_EDIT);
	m_BoundMaxZ = GetDlgItem(hDlg, BOUNDMAXZ_EDIT);

	//List Box 목록 초기화
	int listCount = ListBox_GetCount(m_hList);
	for (int i = 0; i < listCount; i++)
		ListBox_DeleteString(m_hList, i);


	for (GeneralMaterial& elem : (*materials))
	{
		int pos = (int)SendMessage(m_hList, LB_ADDSTRING, 0,
			(LPARAM)elem.name.c_str());

		//GeneralMaterial의 주소값을 data로 설정
		SendMessage(m_hList, LB_SETITEMDATA, pos, (LPARAM)&elem);

	}


}


MeshRendererDialog::MeshRendererDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_MeshRenderer(0)
															,mesh(0), materials(0)
{
	assert(!instantiated);
	instantiated = true;
	g_MeshRendererDialog = this;
}
	


void MeshRendererDialog::MapEditBoxUpdate(int materialIdx)
{
	float* textureTileX = &((*materials)[materialIdx].textureTiling.x);
	float* textureTileY = &((*materials)[materialIdx].textureTiling.y);
	float* textureOffsetX = &((*materials)[materialIdx].textureOffset.x);
	float* textureOffsetY = &((*materials)[materialIdx].textureOffset.y);

	
	controlMap[DIFFUSETILEX_EDIT] = handleFloatPair{ m_hDiffuseTileX, textureTileX };
	controlMap[DIFFUSETILEY_EDIT] = handleFloatPair{ m_hDiffuseTileY, textureTileY };
	controlMap[DIFFUSEOFFSETX_EDIT] = handleFloatPair{ m_hDiffuseOffsetX, textureOffsetX };
	controlMap[DIFFUSEOFFSETY_EDIT] = handleFloatPair{ m_hDiffuseOffsetY, textureOffsetY };

	
	//diffuse map editbox 설정
	Edit_SetText(m_hDiffuseTileX,
		std::to_wstring((*materials)[materialIdx].textureTiling.x).c_str());
	Edit_SetText(m_hDiffuseTileY,
		std::to_wstring((*materials)[materialIdx].textureTiling.y).c_str());
	Edit_SetText(m_hDiffuseOffsetX,
		std::to_wstring((*materials)[materialIdx].textureOffset.x).c_str());
	Edit_SetText(m_hDiffuseOffsetY,
		std::to_wstring((*materials)[materialIdx].textureOffset.y).c_str());

	//normal map editbox 설정, 임시로 1로 설정
	Edit_SetText(m_hNormalTileX, std::to_wstring(1.0f).c_str());
	Edit_SetText(m_hNormalTileY, std::to_wstring(1.0f).c_str());
	Edit_SetText(m_hNormalOffsetX, std::to_wstring(1.0f).c_str());
	Edit_SetText(m_hNormalOffsetY, std::to_wstring(1.0f).c_str());
}

void MeshRendererDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	
	//LOWORD(wParam) = 컨트롤 식별
	int wmId = LOWORD(wParam);

	//texture 행렬 editbox가 수정되었을 때
	for (auto& elem : controlMap)
	{
		if (wmId == elem.first)
		{
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
			{
				WCHAR str[20];
				Edit_GetText(elem.second.first, str, 20);
				
				//해당 editbox와 매칭된 texture 행렬의 값 변경
				(*elem.second.second) = _wtof(str);
			}
				
			}
			
		}
	}



	switch (wmId)
	{
	//파일탐색기에서 파일 선택 후 DiffuseMap 설정
	case DIFFUSEMAPBUTTON:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			WCHAR fileTitle[100];
			WCHAR filePath[100];
			std::vector<LPCWSTR> extensions = { L"dds"};
			if (FileOpenDialog(m_hDlg, fileTitle, filePath, extensions))
			{
				int idx = ListBox_GetCurSel(m_hList);
				if (idx == -1)
					break;
				CommandQueue::AddCommand(new SetMaterialMap(&(*materials)[idx], filePath, mapType::Type_DiffuseMap));
				
				//diffuseMapText에 이름 설정
				Edit_SetText(GetDlgItem(m_hDlg, DIFFUSEMAPCONTROL), fileTitle);
			}
			else
				MessageBox(m_hDlg, L"DiffuseMap Load Fail!", L"DiffuseMap Load", MB_OK);
			
		}
		break;
	case NORMALMAPBUTTON:
		if (HIWORD(wParam) == BN_CLICKED)
		{

		}
		break;
	case  MATERIALLIST:
		//listbox 항목이 선택됐을 때
		if (HIWORD(wParam) == LBN_SELCHANGE)
		{
			int idx = ListBox_GetCurSel(m_hList);
			if (idx == -1)
				break;
			MapEditBoxUpdate(idx);
		}
		break;
	case BLENDINGCHECK:
		//blending check박스 체크
		switch (Button_GetCheck(GetDlgItem(m_hDlg, BLENDINGCHECK)))
		{
		case BST_CHECKED:
			if(m_MeshRenderer!=nullptr)
				m_MeshRenderer->SetBlending(true);
			break;
		case BST_UNCHECKED:
			if (m_MeshRenderer != nullptr)
				m_MeshRenderer->SetBlending(false);
			break;
		}
		
		break;
	}


}





void MeshRendererDialog::KeyDownProc(WPARAM wParam)
{

}


void MeshRendererDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{

}



void MeshRendererDialog::CharProc(HWND hDlg, WPARAM wParam)
{

}

void MeshRendererDialog::NotifyProc(HWND hDlg, LPARAM lParam)
{
	UINT codeTest = ((LPNMHDR)lParam)->code;
	switch (((LPNMHDR)lParam)->code)
	{

		
	case TCN_SELCHANGE:
		
		return;

	}
}