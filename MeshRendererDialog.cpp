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
	m_aabb = mesh->GetAABB_MaxMin();

	return true;
}

bool MeshRendererDialog::UpdateView()
{	
	if (m_MeshRenderer == nullptr)
		return false;
	
	
	
	if (mesh != nullptr)
	{
		static HWND h_meshNameBox = GetDlgItem(m_hDlg, IDC_EDIT11);
		//ASCII -> UNICODE 변환
		USES_CONVERSION;

		WCHAR currentName[100] = L"";
		Edit_GetText(h_meshNameBox, currentName, 100);
		LPCWSTR meshName = A2W(mesh->id.c_str());

		//현재 mesh와 editText의 이름이 다르면
		if (wcscmp(currentName, meshName) != 0)
		{
			//mesh editText에 이름 설정
			Edit_SetText(h_meshNameBox, meshName);
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
	m_hBoundMinX = GetDlgItem(hDlg, BOUNDMINX_EDIT);
	m_hBoundMinY = GetDlgItem(hDlg, BOUNDMINY_EDIT);
	m_hBoundMinZ = GetDlgItem(hDlg, BOUNDMINZ_EDIT);

	m_hBoundMaxX = GetDlgItem(hDlg, BOUNDMAXX_EDIT);
	m_hBoundMaxY = GetDlgItem(hDlg, BOUNDMAXY_EDIT);
	m_hBoundMaxZ = GetDlgItem(hDlg, BOUNDMAXZ_EDIT);

	//color BOX 수정 에디트박스 핸들
	m_hColorR = GetDlgItem(hDlg, COLOR_R_EDIT);
	m_hColorG = GetDlgItem(hDlg, COLOR_G_EDIT);
	m_hColorB = GetDlgItem(hDlg, COLOR_B_EDIT);
	m_hColorA = GetDlgItem(hDlg, COLOR_A_EDIT);

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



	//blending 체크박스 설정
	HWND h_blendingCheck = GetDlgItem(m_hDlg, BLENDINGCHECK);
	Button_SetCheck(h_blendingCheck, m_MeshRenderer->GetBlending());

	//instancing 체크박스 설정
	HWND h_instancingCheck = GetDlgItem(m_hDlg, INSTANCINGCHECK);
	Button_SetCheck(h_instancingCheck, m_MeshRenderer->GetInstancing());

	//Static 체크박스 설정
	HWND h_staticCheck = GetDlgItem(m_hDlg, STATICCHECK);
	Button_SetCheck(h_staticCheck, m_MeshRenderer->GetStaticObject());

	Button_SetCheck(GetDlgItem(m_hDlg, BUILDSHADOWMAPCHECK), m_MeshRenderer->isShadowBaking);
	Button_SetCheck(GetDlgItem(m_hDlg, RECEIVESHADOWCHECK), m_MeshRenderer->isShadowed);
}

bool MeshRendererDialog::printMap()
{
	//컨트롤의 위치,크기 얻어오기
	RECT diffuseControlRect;
	GetWindowRect(GetDlgItem(m_hDlg, DIFFUSEMAPCONTROL), &diffuseControlRect);
	Screen2Client(m_hDlg, &diffuseControlRect);

	RECT normalControlRect;
	GetWindowRect(GetDlgItem(m_hDlg, NORMALMAPCONTROL), &normalControlRect);
	Screen2Client(m_hDlg, &normalControlRect);

	//선택한 텍스쳐를 diffuseControl 박스 위에 그리기
	if (mapNames[0].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[0].first, &diffuseControlRect))
			Edit_SetText(GetDlgItem(m_hDlg, DIFFUSEMAPCONTROL), mapNames[0].second); //diffuseMapText에 이름 설정
	}
	//선택한 텍스쳐를 normalControl 박스 위에 그리기
	if (mapNames[1].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[1].first, &normalControlRect))
			Edit_SetText(GetDlgItem(m_hDlg, NORMALMAPCONTROL), mapNames[1].second); //diffuseMapText에 이름 설정
	}
		
	return true;
}


MeshRendererDialog::MeshRendererDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_MeshRenderer(0)
															,mesh(0), materials(0)
{
	assert(!instantiated);
	instantiated = true;
	g_MeshRendererDialog = this;
	
	mapNames[0].first = new WCHAR[100];
	mapNames[0].second = new WCHAR[100];
	mapNames[1].first = new WCHAR[100];
	mapNames[1].second = new WCHAR[100];
	wcscpy(mapNames[0].first, L"NULL");
	wcscpy(mapNames[0].second, L"NULL");
	wcscpy(mapNames[1].first, L"NULL");
	wcscpy(mapNames[1].second, L"NULL");
}

MeshRendererDialog::~MeshRendererDialog()
{
	delete[] mapNames[0].first;
	delete[] mapNames[0].second;
	delete[] mapNames[1].first;
	delete[] mapNames[1].second;
}
	

//Material이 선택되면 각 EditBox를 업데이트함.
void MeshRendererDialog::MapEditBoxUpdate(int materialIdx)
{
	//텍스쳐행렬 EditBox의 컨트롤 id와 수정해야할 값의 주소를 매핑
	float* textureTileX = &((*materials)[materialIdx].textureTiling.x);
	float* textureTileY = &((*materials)[materialIdx].textureTiling.y);
	float* textureOffsetX = &((*materials)[materialIdx].textureOffset.x);
	float* textureOffsetY = &((*materials)[materialIdx].textureOffset.y);
		
	controlMap[DIFFUSETILEX_EDIT] = handleFloatPair{ m_hDiffuseTileX, textureTileX };
	controlMap[DIFFUSETILEY_EDIT] = handleFloatPair{ m_hDiffuseTileY, textureTileY };
	controlMap[DIFFUSEOFFSETX_EDIT] = handleFloatPair{ m_hDiffuseOffsetX, textureOffsetX };
	controlMap[DIFFUSEOFFSETY_EDIT] = handleFloatPair{ m_hDiffuseOffsetY, textureOffsetY };

	//바운딩박스 EdidtBox~위와 동일
	float* aabbMaxX = &(m_aabb.m_max.x);
	float* aabbMaxY = &(m_aabb.m_max.y);
	float* aabbMaxZ = &(m_aabb.m_max.z);
	
	float* aabbMinX = &(m_aabb.m_min.x);
	float* aabbMinY = &(m_aabb.m_min.y);
	float* aabbMinZ = &(m_aabb.m_min.z);


	controlMap[BOUNDMAXX_EDIT] = handleFloatPair{ m_hBoundMaxX, aabbMaxX };
	controlMap[BOUNDMAXY_EDIT] = handleFloatPair{ m_hBoundMaxY, aabbMaxY };
	controlMap[BOUNDMAXZ_EDIT] = handleFloatPair{ m_hBoundMaxZ, aabbMaxZ };

	controlMap[BOUNDMINX_EDIT] = handleFloatPair{ m_hBoundMinX, aabbMinX };
	controlMap[BOUNDMINY_EDIT] = handleFloatPair{ m_hBoundMinY, aabbMinY };
	controlMap[BOUNDMINZ_EDIT] = handleFloatPair{ m_hBoundMinZ, aabbMinZ };

	//color 에디트박스 위와 동일
	float *r = &m_MeshRenderer->m_color.x;
	float *g = &m_MeshRenderer->m_color.y;
	float *b = &m_MeshRenderer->m_color.z;
	float *a = &m_MeshRenderer->m_color.w;

	controlMap[COLOR_R_EDIT] = handleFloatPair{ m_hColorR, r };
	controlMap[COLOR_G_EDIT] = handleFloatPair{ m_hColorG, g };
	controlMap[COLOR_B_EDIT] = handleFloatPair{ m_hColorB, b };
	controlMap[COLOR_A_EDIT] = handleFloatPair{ m_hColorA, a };

	//diffuse, normal map들 이름 설정
	std::wstring& diffuseMap = (*materials)[materialIdx].diffuseMapName;
	std::wstring& normalMap = (*materials)[materialIdx].normalMapName;
	//파일경로와 파일명 설정
	wcscpy(mapNames[0].first, diffuseMap.c_str());
	wcscpy(mapNames[0].second, ExtractTitle(diffuseMap).c_str());
	wcscpy(mapNames[1].first, normalMap.c_str());
	wcscpy(mapNames[1].second, ExtractTitle(normalMap).c_str());
	
	printMap();
	
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
	
	//bounding Box editbox 설정
	Edit_SetText(m_hBoundMaxX, std::to_wstring(*aabbMaxX).c_str());
	Edit_SetText(m_hBoundMaxY, std::to_wstring(*aabbMaxY).c_str());
	Edit_SetText(m_hBoundMaxZ, std::to_wstring(*aabbMaxZ).c_str());
	Edit_SetText(m_hBoundMinX, std::to_wstring(*aabbMinX).c_str());
	Edit_SetText(m_hBoundMinY, std::to_wstring(*aabbMinY).c_str());
	Edit_SetText(m_hBoundMinZ, std::to_wstring(*aabbMinZ).c_str());

	//color editbox 설정
	Edit_SetText(m_hColorR, std::to_wstring(*r).c_str());
	Edit_SetText(m_hColorG, std::to_wstring(*g).c_str());
	Edit_SetText(m_hColorB, std::to_wstring(*b).c_str());
	Edit_SetText(m_hColorA, std::to_wstring(*a).c_str());
}

void MeshRendererDialog::MenuProc(HWND hDlg, WPARAM wParam)
{
	
	//LOWORD(wParam) = 컨트롤 식별
	int wmId = LOWORD(wParam);
	
	//editbox가 수정되었을 때
	for (auto& elem : controlMap)
	{
		if (wmId == elem.first)
		{	
			switch(HIWORD(wParam))
			{
			case EN_CHANGE:
			{
				WCHAR str[30];
				//Handle을 통해 텍스트를 얻어옴.
				Edit_GetText(elem.second.first, str, 30);
				
				//해당 editbox와 매칭된 값 변경
				(*elem.second.second) = _wtof(str);

				//boundingBox 수정일 때
				if (wmId == BOUNDMAXX_EDIT || wmId == BOUNDMAXY_EDIT || wmId == BOUNDMAXZ_EDIT ||
					wmId == BOUNDMINX_EDIT || wmId == BOUNDMINY_EDIT || wmId == BOUNDMINZ_EDIT)
				{
					mesh->SetAABB_MaxMin(m_aabb.m_max, m_aabb.m_min);
				}
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
			std::vector<LPCWSTR> extensions = { L"dds", L"bmp"};
			if (FileOpenDialog(m_hDlg, fileTitle, filePath, extensions))
			{
				int idx = ListBox_GetCurSel(m_hList);
				if (idx == -1)
					break;
				CommandQueue::AddCommand(new SetMaterialMap(m_MeshRenderer, idx, filePath, mapType::Type_DiffuseMap));
				
				wcscpy(mapNames[0].first, filePath);
				wcscpy(mapNames[0].second, fileTitle);
								
				//선택한 texture를 dialog에 그리기
				printMap();
			}
			else
				MessageBox(m_hDlg, L"DiffuseMap Load Fail!", L"DiffuseMap Load", MB_OK);
			
		}
		break;
	case NORMALMAPBUTTON:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			WCHAR fileTitle[100];
			WCHAR filePath[100];
			std::vector<LPCWSTR> extensions = { L"dds", L"bmp" };
			if (FileOpenDialog(m_hDlg, fileTitle, filePath, extensions))
			{
				int idx = ListBox_GetCurSel(m_hList);
				if (idx == -1)
					break;
				CommandQueue::AddCommand(new SetMaterialMap(m_MeshRenderer,idx, filePath, mapType::Type_NormalMap));

				wcscpy(mapNames[1].first, filePath);
				wcscpy(mapNames[1].second, fileTitle);

				//선택한 texture를 dialog에 그리기
				printMap();
			}
			else
				MessageBox(m_hDlg, L"NormalMap Load Fail!", L"NormalMap Load", MB_OK);
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
	case INSTANCINGCHECK:
		//Instancing check박스 체크
		switch (Button_GetCheck(GetDlgItem(m_hDlg, INSTANCINGCHECK)))
		{
			case BST_CHECKED:
				if (m_MeshRenderer != nullptr)
					m_MeshRenderer->SetInstancing(true);
				break;
			case BST_UNCHECKED:
				if (m_MeshRenderer != nullptr)
					m_MeshRenderer->SetInstancing(false);
				break;
		}
		break;
	case STATICCHECK:
		//static check박스 체크
		switch (Button_GetCheck(GetDlgItem(m_hDlg, STATICCHECK)))
		{
		case BST_CHECKED:
			if (m_MeshRenderer != nullptr)
				CommandQueue::AddCommand(new OctreeAddObject(m_MeshRenderer));
			break;
		case BST_UNCHECKED:
			if (m_MeshRenderer != nullptr)
				CommandQueue::AddCommand(new OctreePopObject(m_MeshRenderer));
			break;
		}
		break;
	case BUILDSHADOWMAPCHECK:
		switch (Button_GetCheck(GetDlgItem(hDlg, BUILDSHADOWMAPCHECK)))
		{
		case BST_CHECKED:
			if (m_MeshRenderer != nullptr)
				m_MeshRenderer->isShadowBaking = true;
			break;
		case BST_UNCHECKED:
			if (m_MeshRenderer != nullptr)
				m_MeshRenderer->isShadowBaking = false;
			break;
		}
		break;
	case RECEIVESHADOWCHECK:
		switch (Button_GetCheck(GetDlgItem(hDlg, RECEIVESHADOWCHECK)))
		{
		case BST_CHECKED:
			if (m_MeshRenderer != nullptr)
				m_MeshRenderer->isShadowed = true;
			break;
		case BST_UNCHECKED:
			if (m_MeshRenderer != nullptr)
				m_MeshRenderer->isShadowed = false;
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