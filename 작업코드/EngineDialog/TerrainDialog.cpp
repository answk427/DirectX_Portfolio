#include "TerrainDialog.h"

extern TerrainDialog* g_TerrainDialog = 0;
bool TerrainDialog::instantiated = false;


//-------------------*ChildProc Test *-----------------------
VOID WINAPI TerrainDialog::OnChildDialogInit(HWND hwndDlg)
{
	//�θ� �������� ��ġ,ũ��
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


INT_PTR CALLBACK TerrainRendererProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_CLOSE:
		EndDialog(hDlg, 0);
		return (INT_PTR)TRUE;
	case WM_INITDIALOG:
	{
		g_TerrainDialog->OnChildDialogInit(hDlg);
		g_TerrainDialog->Init(hDlg);


		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
		g_TerrainDialog->MenuProc(hDlg, wParam);
		return (INT_PTR)TRUE;
	case WM_KEYDOWN:

		return (INT_PTR)TRUE;
	case WM_MOUSEMOVE:

		return (INT_PTR)TRUE;
	case WM_NOTIFY: //����Ʈ ��Ʈ�� Ŭ������ �� �߻�
		g_TerrainDialog->NotifyProc(hDlg, lParam);
		return (INT_PTR)TRUE;
	case WM_MOVE: //
	{

		return (INT_PTR)TRUE;
	}


	}
	return (INT_PTR)FALSE;
}
//-------------------*ChildProc Test *-----------------------



bool TerrainDialog::OpenDialog()
{
	return false;
}

bool TerrainDialog::SetObject(GameObject* obj)
{
	TerrainRenderer* terrainRenderer = obj->GetComponent<TerrainRenderer>();
	m_TerrainRenderer = terrainRenderer;
	if (!m_TerrainRenderer)
		return false;

	materials = &(m_TerrainRenderer->GetMaterials());
	mesh = m_TerrainRenderer->GetMesh();
	m_aabb = mesh->GetAABB_MaxMin();
		
	return true;
}

bool TerrainDialog::UpdateView()
{
	if (m_TerrainRenderer == nullptr)
		return false;



	//if (mesh != nullptr)
	//{
	//	static HWND h_meshNameBox = GetDlgItem(m_hDlg, IDC_EDIT11);
	//	//ASCII -> UNICODE ��ȯ
	//	USES_CONVERSION;

	//	WCHAR currentName[100] = L"";
	//	Edit_GetText(h_meshNameBox, currentName, 100);
	//	LPCWSTR meshName = A2W(mesh->id.c_str());

	//	//���� mesh�� editText�� �̸��� �ٸ���
	//	if (wcscmp(currentName, meshName) != 0)
	//	{
	//		//mesh editText�� �̸� ����
	//		Edit_SetText(h_meshNameBox, meshName);
	//	}

	//}


	return true;

}


bool TerrainDialog::OpenDialog(HWND hwnd)
{

	m_hwndParent = hwnd;

	if (!IsWindow(m_hDlg))
	{
		m_hDlg = CreateDialog(m_hInstance, MAKEINTRESOURCE(TAB_TERRAINRENDERER), hwnd, TerrainRendererProc);

		ShowWindow(m_hDlg, SW_SHOW);
		return true;
	}

	return false;
}

void TerrainDialog::Init(HWND hDlg)
{
	m_hDlg = hDlg;

	USES_CONVERSION;
	wchar_t itemName[100];
	
	HWND hCombo = GetDlgItem(hDlg, HEIGHTMAPSIZE_COMBO);
	std::wstring str;
	//combo�ڽ��� item �߰�
	for (auto elem : TerrainRenderer::HeightMapSizes)
	{
		str = std::to_wstring(elem);
		swprintf(itemName, L"%s", str.c_str());
		ComboBox_AddString(hCombo, itemName);
	}

	HWND hShapeCombo = GetDlgItem(hDlg, BRUSHSHAPE_COMBO);
	ComboBox_AddString(hShapeCombo, TEXT("NOBRUSH"));
	ComboBox_AddString(hShapeCombo, TEXT("SQUARE"));
	ComboBox_AddString(hShapeCombo, TEXT("CIRCLE"));

	////material �ؽ������ ���� ����Ʈ�ڽ� �ڵ�
	m_hDiffuseTileX = GetDlgItem(hDlg, TEXTURETILEX_EDIT);
	m_hDiffuseTileY = GetDlgItem(hDlg, TEXTURETILEY_EDIT);
	m_hDiffuseOffsetX = GetDlgItem(hDlg, TEXTUREOFFSETX_EDIT);
	m_hDiffuseOffsetY = GetDlgItem(hDlg, TEXTUREOFFSETY_EDIT);
	
	////BOUNDING BOX ���� ����Ʈ�ڽ� �ڵ�
	//m_hBoundMinX = GetDlgItem(hDlg, BOUNDMINX_EDIT);
	//m_hBoundMinY = GetDlgItem(hDlg, BOUNDMINY_EDIT);
	//m_hBoundMinZ = GetDlgItem(hDlg, BOUNDMINZ_EDIT);

	//m_hBoundMaxX = GetDlgItem(hDlg, BOUNDMAXX_EDIT);
	//m_hBoundMaxY = GetDlgItem(hDlg, BOUNDMAXY_EDIT);
	//m_hBoundMaxZ = GetDlgItem(hDlg, BOUNDMAXZ_EDIT);

	TerrainRenderer::TerrainInitInfo& terrainData = m_TerrainRenderer->m_terrainData;
	Edit_SetText(GetDlgItem(hDlg, HEIGHTSCALE_EDIT), std::to_wstring(terrainData.HeightScale).c_str());
	Edit_SetText(GetDlgItem(hDlg, CELLSPACING_EDIT), std::to_wstring(terrainData.CellSpacing).c_str());
	Edit_SetText(GetDlgItem(hDlg, CELLSPERPATCH_EDIT), std::to_wstring(m_TerrainRenderer->GetCellsPerPatch()).c_str());
	
	Edit_SetText(GetDlgItem(hDlg, FOGSTART_EDIT), std::to_wstring(m_TerrainRenderer->GetFogStart()).c_str());
	Edit_SetText(GetDlgItem(hDlg, FOGRANGE_EDIT), std::to_wstring(m_TerrainRenderer->GetFogRange()).c_str());

	TesselationData& td = m_TerrainRenderer->m_tesselationData;
	Edit_SetText(GetDlgItem(hDlg, MINDIST_EDIT), std::to_wstring(td.GetMinDist()).c_str());
	Edit_SetText(GetDlgItem(hDlg, MAXDIST_EDIT), std::to_wstring(td.GetMaxDist()).c_str());
	Edit_SetText(GetDlgItem(hDlg, MINTESS_EDIT), std::to_wstring(td.GetMinTess()).c_str());
	Edit_SetText(GetDlgItem(hDlg, MAXTESS_EDIT), std::to_wstring(td.GetMaxTess()).c_str());

	////color BOX ���� ����Ʈ�ڽ� �ڵ�
	//m_hColorR = GetDlgItem(hDlg, COLOR_R_EDIT);
	//m_hColorG = GetDlgItem(hDlg, COLOR_G_EDIT);
	//m_hColorB = GetDlgItem(hDlg, COLOR_B_EDIT);
	//m_hColorA = GetDlgItem(hDlg, COLOR_A_EDIT);

	
	////blending üũ�ڽ� ����
	//HWND h_blendingCheck = GetDlgItem(m_hDlg, BLENDINGCHECK);
	//Button_SetCheck(h_blendingCheck, m_TerrainRenderer->GetBlending());

	////instancing üũ�ڽ� ����
	//HWND h_instancingCheck = GetDlgItem(m_hDlg, INSTANCINGCHECK);
	//Button_SetCheck(h_instancingCheck, m_TerrainRenderer->GetInstancing());

	//EnableFog üũ�ڽ� ����
	HWND h_fogCheck = GetDlgItem(m_hDlg, FOGENABLE_CHECK);
	Button_SetCheck(h_fogCheck, m_TerrainRenderer->GetEnableFog());

	Button_SetCheck(GetDlgItem(m_hDlg, BUILDSHADOWMAPBUTTON_TERRAIN),
		m_TerrainRenderer->isShadowBaking);
	Button_SetCheck(GetDlgItem(m_hDlg, RECEIVESHADOWBUTTON_TERRAIN),
		m_TerrainRenderer->isShadowed);


	
	//���ϰ�ο� ���ϸ� ����
	TerrainRenderer::TerrainInitInfo& terrainInfo = m_TerrainRenderer->m_terrainData;
	for (int i = 0; i < terrainInfo.layerFilenames.size(); ++i)
	{
		wcscpy(mapNames[i].first, terrainInfo.layerFilenames[i].c_str());
		wcscpy(mapNames[i].second, ExtractTitle(terrainInfo.layerFilenames[i]).c_str());
	}
		

	MapEditBoxUpdate(0);
}

bool TerrainDialog::printMap()
{
	//��Ʈ���� ��ġ,ũ�� ������
	RECT basicMapRect;
	GetWindowRect(GetDlgItem(m_hDlg, BASICMAPCONTROL), &basicMapRect);
	Screen2Client(m_hDlg, &basicMapRect);

	RECT layerMapRect1;
	GetWindowRect(GetDlgItem(m_hDlg, LAYERMAPCONTROL1), &layerMapRect1);
	Screen2Client(m_hDlg, &layerMapRect1);

	RECT layerMapRect2;
	GetWindowRect(GetDlgItem(m_hDlg, LAYERMAPCONTROL2), &layerMapRect2);
	Screen2Client(m_hDlg, &layerMapRect2);

	RECT layerMapRect3;
	GetWindowRect(GetDlgItem(m_hDlg, LAYERMAPCONTROL3), &layerMapRect3);
	Screen2Client(m_hDlg, &layerMapRect3);

	RECT layerMapRect4;
	GetWindowRect(GetDlgItem(m_hDlg, LAYERMAPCONTROL4), &layerMapRect4);
	Screen2Client(m_hDlg, &layerMapRect4);

	RECT blendMapRect;
	GetWindowRect(GetDlgItem(m_hDlg, BLENDMAPCONTROL), &blendMapRect);
	Screen2Client(m_hDlg, &blendMapRect);

	//������ �ؽ��ĸ� �ڽ� ���� �׸���
	if (mapNames[0].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[0].first, &basicMapRect))
			Edit_SetText(GetDlgItem(m_hDlg, BASICMAPCONTROL), mapNames[0].second); //diffuseMapText�� �̸� ����
	}
	if (mapNames[1].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[1].first, &layerMapRect1))
			Edit_SetText(GetDlgItem(m_hDlg, LAYERMAPCONTROL1), mapNames[1].second); //diffuseMapText�� �̸� ����
	}
	if (mapNames[2].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[2].first, &layerMapRect2))
			Edit_SetText(GetDlgItem(m_hDlg, LAYERMAPCONTROL2), mapNames[2].second); //diffuseMapText�� �̸� ����
	}
	if (mapNames[3].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[3].first, &layerMapRect3))
			Edit_SetText(GetDlgItem(m_hDlg, LAYERMAPCONTROL3), mapNames[3].second); //diffuseMapText�� �̸� ����
	}
	if (mapNames[4].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[4].first, &layerMapRect4))
			Edit_SetText(GetDlgItem(m_hDlg, LAYERMAPCONTROL4), mapNames[4].second); //diffuseMapText�� �̸� ����
	}
	if (mapNames[5].first != nullptr)
	{
		if (!PrintCImage(GetDC(m_hDlg), mapNames[5].first, &blendMapRect))
			Edit_SetText(GetDlgItem(m_hDlg, BLENDMAPCONTROL), mapNames[5].second); //diffuseMapText�� �̸� ����
	}

	return true;
}


TerrainDialog::TerrainDialog(HINSTANCE hInstance) : ComponentDialog(hInstance), m_TerrainRenderer(0)
, mesh(0), materials(0)
{
	assert(!instantiated);
	instantiated = true;
	g_TerrainDialog = this;

	for (int i = 0; i < 6; ++i)
	{
		mapNames[i].first = new WCHAR[100];
		mapNames[i].second = new WCHAR[100];
		wcscpy(mapNames[i].first, L"NULL");
		wcscpy(mapNames[i].second, L"NULL");
	}
		
}

TerrainDialog::~TerrainDialog()
{
	for (int i = 0; i < 6; ++i)
	{
		delete[] mapNames[i].first;
		delete[] mapNames[i].second;	
	}
}


//Material�� ���õǸ� �� EditBox�� ������Ʈ��.
void TerrainDialog::MapEditBoxUpdate(int materialIdx)
{
	//�ؽ������ EditBox�� ��Ʈ�� id�� �����ؾ��� ���� �ּҸ� ����
	float* textureTileX = &((*materials)[materialIdx].textureTiling.x);
	float* textureTileY = &((*materials)[materialIdx].textureTiling.y);
	float* textureOffsetX = &((*materials)[materialIdx].textureOffset.x);
	float* textureOffsetY = &((*materials)[materialIdx].textureOffset.y);

	controlMap[TEXTURETILEX_EDIT] = handleFloatPair{ m_hDiffuseTileX, textureTileX };
	controlMap[TEXTURETILEY_EDIT] = handleFloatPair{ m_hDiffuseTileY, textureTileY };
	controlMap[TEXTUREOFFSETX_EDIT] = handleFloatPair{ m_hDiffuseOffsetX, textureOffsetX };
	controlMap[TEXTUREOFFSETY_EDIT] = handleFloatPair{ m_hDiffuseOffsetY, textureOffsetY };

	////�ٿ���ڽ� EdidtBox~���� ����
	//float* aabbMaxX = &(m_aabb.m_max.x);
	//float* aabbMaxY = &(m_aabb.m_max.y);
	//float* aabbMaxZ = &(m_aabb.m_max.z);

	//float* aabbMinX = &(m_aabb.m_min.x);
	//float* aabbMinY = &(m_aabb.m_min.y);
	//float* aabbMinZ = &(m_aabb.m_min.z);


	//controlMap[BOUNDMAXX_EDIT] = handleFloatPair{ m_hBoundMaxX, aabbMaxX };
	//controlMap[BOUNDMAXY_EDIT] = handleFloatPair{ m_hBoundMaxY, aabbMaxY };
	//controlMap[BOUNDMAXZ_EDIT] = handleFloatPair{ m_hBoundMaxZ, aabbMaxZ };

	//controlMap[BOUNDMINX_EDIT] = handleFloatPair{ m_hBoundMinX, aabbMinX };
	//controlMap[BOUNDMINY_EDIT] = handleFloatPair{ m_hBoundMinY, aabbMinY };
	//controlMap[BOUNDMINZ_EDIT] = handleFloatPair{ m_hBoundMinZ, aabbMinZ };

	////color ����Ʈ�ڽ� ���� ����
	//float *r = &m_TerrainRenderer->m_color.x;
	//float *g = &m_TerrainRenderer->m_color.y;
	//float *b = &m_TerrainRenderer->m_color.z;
	//float *a = &m_TerrainRenderer->m_color.w;

	//controlMap[COLOR_R_EDIT] = handleFloatPair{ m_hColorR, r };
	//controlMap[COLOR_G_EDIT] = handleFloatPair{ m_hColorG, g };
	//controlMap[COLOR_B_EDIT] = handleFloatPair{ m_hColorB, b };
	//controlMap[COLOR_A_EDIT] = handleFloatPair{ m_hColorA, a };

	////diffuse, normal map�� �̸� ����
	//std::wstring& diffuseMap = (*materials)[materialIdx].diffuseMapName;
	//std::wstring& normalMap = (*materials)[materialIdx].normalMapName;
	////���ϰ�ο� ���ϸ� ����
	//wcscpy(mapNames[0].first, diffuseMap.c_str());
	//wcscpy(mapNames[0].second, ExtractTitle(diffuseMap).c_str());
	//wcscpy(mapNames[1].first, normalMap.c_str());
	//wcscpy(mapNames[1].second, ExtractTitle(normalMap).c_str());

	printMap();


	//diffuse map editbox ����
	Edit_SetText(m_hDiffuseTileX,
		std::to_wstring((*materials)[materialIdx].textureTiling.x).c_str());
	Edit_SetText(m_hDiffuseTileY,
		std::to_wstring((*materials)[materialIdx].textureTiling.y).c_str());
	Edit_SetText(m_hDiffuseOffsetX,
		std::to_wstring((*materials)[materialIdx].textureOffset.x).c_str());
	Edit_SetText(m_hDiffuseOffsetY,
		std::to_wstring((*materials)[materialIdx].textureOffset.y).c_str());

	
	////bounding Box editbox ����
	//Edit_SetText(m_hBoundMaxX, std::to_wstring(*aabbMaxX).c_str());
	//Edit_SetText(m_hBoundMaxY, std::to_wstring(*aabbMaxY).c_str());
	//Edit_SetText(m_hBoundMaxZ, std::to_wstring(*aabbMaxZ).c_str());
	//Edit_SetText(m_hBoundMinX, std::to_wstring(*aabbMinX).c_str());
	//Edit_SetText(m_hBoundMinY, std::to_wstring(*aabbMinY).c_str());
	//Edit_SetText(m_hBoundMinZ, std::to_wstring(*aabbMinZ).c_str());

	////color editbox ����
	//Edit_SetText(m_hColorR, std::to_wstring(*r).c_str());
	//Edit_SetText(m_hColorG, std::to_wstring(*g).c_str());
	//Edit_SetText(m_hColorB, std::to_wstring(*b).c_str());
	//Edit_SetText(m_hColorA, std::to_wstring(*a).c_str());
}

void TerrainDialog::MenuProc(HWND hDlg, WPARAM wParam)
{

	//LOWORD(wParam) = ��Ʈ�� �ĺ�
	int wmId = LOWORD(wParam);

	//editbox�� �����Ǿ��� �� (Texture Tiling)
	for (auto& elem : controlMap)
	{
		if (wmId == elem.first)
		{
			switch (HIWORD(wParam))
			{
			case EN_CHANGE:
			{
				WCHAR str[30];
				//Handle�� ���� �ؽ�Ʈ�� ����.
				Edit_GetText(elem.second.first, str, 30);

				//�ش� editbox�� ��Ī�� �� ����
				(*elem.second.second) = _wtof(str);			
			}
			}
		}
	}


	//����Ž���⿡�� ���� ���� �� DiffuseMap ����
	static std::vector<std::pair<UINT, UINT>> imgButtonPairs =
	{ {BASICMAPBUTTON,BASICMAPCONTROL},{LAYERMAPBUTTON1,LAYERMAPCONTROL1},{LAYERMAPBUTTON2,LAYERMAPCONTROL2},
	{LAYERMAPBUTTON3,LAYERMAPCONTROL3} ,{LAYERMAPBUTTON4,LAYERMAPCONTROL4}, {BLENDMAPBUTTON, BLENDMAPCONTROL} };

	//��Ʈ���� ���� ���� �Ǿ��� ��
	if (HIWORD(wParam) == EN_CHANGE)
	{
		switch (wmId)
		{
		case HEIGHTSCALE_EDIT:
			m_TerrainRenderer->SetHeightScale(EditText2Float(GetDlgItem(hDlg,HEIGHTSCALE_EDIT)));
			break;
		case CELLSPACING_EDIT:
			m_TerrainRenderer->SetCellSpacing(EditText2Float(GetDlgItem(hDlg, CELLSPACING_EDIT)));
			break;
		case CELLSPERPATCH_EDIT:
			m_TerrainRenderer->SetCellsPerPatch(EditText2Float(GetDlgItem(hDlg, CELLSPERPATCH_EDIT)));
			break;
		case FOGSTART_EDIT:
			m_TerrainRenderer->SetFogStart(EditText2Float(GetDlgItem(hDlg, FOGSTART_EDIT)));
			break;
		case FOGRANGE_EDIT:
			m_TerrainRenderer->SetFogRange(EditText2Float(GetDlgItem(hDlg, FOGRANGE_EDIT)));
			break;
		case MINDIST_EDIT:
			m_TerrainRenderer->m_tesselationData.SetMinDist(EditText2Float(GetDlgItem(hDlg, MINDIST_EDIT)));
			break;
		case MAXDIST_EDIT:
			m_TerrainRenderer->m_tesselationData.SetMaxDist(EditText2Float(GetDlgItem(hDlg, MAXDIST_EDIT)));
			break;
		case MINTESS_EDIT:
			m_TerrainRenderer->m_tesselationData.SetMinTess(EditText2Float(GetDlgItem(hDlg, MINTESS_EDIT)));
			break;
		case MAXTESS_EDIT:
			m_TerrainRenderer->m_tesselationData.SetMaxTess(EditText2Float(GetDlgItem(hDlg, MAXTESS_EDIT)));
			break;
		case BRUSHRADIUS_EDIT:
			m_TerrainRenderer->m_brush->radius = EditText2Float(GetDlgItem(hDlg, BRUSHRADIUS_EDIT));
			break;
		
		}
	}

	switch (wmId)
	{				
	case FOGENABLE_CHECK:
		//blending check�ڽ� üũ
		switch (Button_GetCheck(GetDlgItem(m_hDlg, FOGENABLE_CHECK)))
		{
		case BST_CHECKED:
			if (m_TerrainRenderer != nullptr)
				m_TerrainRenderer->SetEnableFog(true);
			break;
		case BST_UNCHECKED:
			if (m_TerrainRenderer != nullptr)
				m_TerrainRenderer->SetEnableFog(false);
			break;
		}
		break;
	case BUILDSHADOWMAPBUTTON_TERRAIN:
		//BuildShadowMap check�ڽ� üũ
		switch (Button_GetCheck(GetDlgItem(m_hDlg, BUILDSHADOWMAPBUTTON_TERRAIN)))
		{
		case BST_CHECKED:
			if (m_TerrainRenderer != nullptr)
				m_TerrainRenderer->isShadowBaking = true;
			break;
		case BST_UNCHECKED:
			if (m_TerrainRenderer != nullptr)
				m_TerrainRenderer->isShadowBaking = false;
			break;
		}
		break;
	case RECEIVESHADOWBUTTON_TERRAIN:
		//ReceiveShadow check�ڽ� üũ
		switch (Button_GetCheck(GetDlgItem(m_hDlg, RECEIVESHADOWBUTTON_TERRAIN)))
		{
		case BST_CHECKED:
			if (m_TerrainRenderer != nullptr)
				m_TerrainRenderer->isShadowed= true;
			break;
		case BST_UNCHECKED:
			if (m_TerrainRenderer != nullptr)
				m_TerrainRenderer->isShadowed= false;
			break;
		}
		break;
		
	case HEIGHTMAP_BUTTON:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			WCHAR fileTitle[200];
			WCHAR filePath[200];
			std::vector<LPCWSTR> extensions = { L"raw" };
			if (FileOpenDialog(m_hDlg, fileTitle, filePath, extensions))
			{
				m_TerrainRenderer->SetHeightMap(std::wstring(filePath));
			}
		}
		break;
	case BRUSHSHAPE_BUTTON:
		if (HIWORD(wParam) == BN_CLICKED)
		{
			m_TerrainRenderer->m_brush->shape = ComboBox_GetCurSel(GetDlgItem(hDlg, BRUSHSHAPE_COMBO));
		}
		break;
		
	//case BLENDINGCHECK:
	//	//blending check�ڽ� üũ
	//	switch (Button_GetCheck(GetDlgItem(m_hDlg, BLENDINGCHECK)))
	//	{
	//	case BST_CHECKED:
	//		if (m_TerrainRenderer != nullptr)
	//			m_TerrainRenderer->SetBlending(true);
	//		break;
	//	case BST_UNCHECKED:
	//		if (m_TerrainRenderer != nullptr)
	//			m_TerrainRenderer->SetBlending(false);
	//		break;
	//	}
	//	break;
	//case INSTANCINGCHECK:
	//	//Instancing check�ڽ� üũ
	//	switch (Button_GetCheck(GetDlgItem(m_hDlg, INSTANCINGCHECK)))
	//	{
	//	case BST_CHECKED:
	//		if (m_TerrainRenderer != nullptr)
	//			m_TerrainRenderer->SetInstancing(true);
	//		break;
	//	case BST_UNCHECKED:
	//		if (m_TerrainRenderer != nullptr)
	//			m_TerrainRenderer->SetInstancing(false);
	//		break;
	//	}
	//	break;
	//case STATICCHECK:
	//	//static check�ڽ� üũ
	//	switch (Button_GetCheck(GetDlgItem(m_hDlg, STATICCHECK)))
	//	{
	//	case BST_CHECKED:
	//		if (m_TerrainRenderer != nullptr)
	//			CommandQueue::AddCommand(new OctreeAddObject(m_TerrainRenderer));
	//		break;
	//	case BST_UNCHECKED:
	//		if (m_TerrainRenderer != nullptr)
	//			CommandQueue::AddCommand(new OctreePopObject(m_TerrainRenderer));
	//		break;
	//	}
	//	break;
	default:
	{
		for (int i = 0; i < imgButtonPairs.size(); ++i)
		{
			if (wmId == imgButtonPairs[i].first)
			{
				if (HIWORD(wParam) == BN_CLICKED)
				{
					WCHAR fileTitle[200];
					WCHAR filePath[200];
					std::vector<LPCWSTR> extensions = { L"bmp",L"jpg",L"dds" };
					if (FileOpenDialog(m_hDlg, fileTitle, filePath, extensions))
					{
						if (i == 5)
							m_TerrainRenderer->SetBlendMap(std::wstring(filePath));
						else
							m_TerrainRenderer->SetLayerMap(std::wstring(filePath), i);

						wcscpy(mapNames[i].first, filePath);
						wcscpy(mapNames[i].second, fileTitle);

						//������ texture�� dialog�� �׸���
						printMap();
					}
					else
						MessageBox(m_hDlg, L"DiffuseMap Load Fail!", L"DiffuseMap Load", MB_OK);

				}
				break;
			}
		}
	}
	}
}





void TerrainDialog::KeyDownProc(WPARAM wParam)
{

}


void TerrainDialog::MouseMoveProc(WPARAM btnState, LPARAM mousePos)
{

}



void TerrainDialog::CharProc(HWND hDlg, WPARAM wParam)
{

}

void TerrainDialog::NotifyProc(HWND hDlg, LPARAM lParam)
{
	UINT codeTest = ((LPNMHDR)lParam)->code;
	switch (((LPNMHDR)lParam)->code)
	{


	case TCN_SELCHANGE:

		return;

	}
}