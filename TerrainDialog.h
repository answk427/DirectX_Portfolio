#pragma once

#include "stdafx.h"
#include <CommCtrl.h>
#include <resource.h>
#include <cassert>

#include "Terrain.h"
#include "ComponentDialog.h"
#include "GameObject.h"
#include "DialogStructureEX.h"
#include "DialogSizeDefine.h"
#include "MeshRendererCommand.h"



#define TAB_TERRAINRENDERER IDD_DIALOG8

#pragma region MapControls
#define TEXTURETILEX_EDIT IDC_EDIT3
#define TEXTURETILEY_EDIT IDC_EDIT4
#define TEXTUREOFFSETX_EDIT IDC_EDIT5
#define TEXTUREOFFSETY_EDIT IDC_EDIT6

#define BASICMAPCONTROL IDC_EDIT16
#define LAYERMAPCONTROL1 IDC_EDIT2
#define LAYERMAPCONTROL2 IDC_EDIT13
#define LAYERMAPCONTROL3 IDC_EDIT14
#define LAYERMAPCONTROL4 IDC_EDIT15
#define BLENDMAPCONTROL IDC_EDIT7


#define BASICMAPBUTTON IDC_BUTTON1
#define LAYERMAPBUTTON1 IDC_BUTTON2
#define LAYERMAPBUTTON2 IDC_BUTTON4
#define LAYERMAPBUTTON3 IDC_BUTTON5
#define LAYERMAPBUTTON4 IDC_BUTTON6
#define BLENDMAPBUTTON	IDC_BUTTON7

#define LAYERMAPRADIO1 IDC_RADIO1
#define LAYERMAPRADIO2 IDC_RADIO4
#define LAYERMAPRADIO3 IDC_RADIO5
#define LAYERMAPRADIO4 IDC_RADIO6

#define RAISEBUTTON IDC_RADIO2
#define DOWNBUTTON IDC_RADIO3

#define NORMALMODERADIO IDC_RADIO7
#define HEIGHTMODERADIO IDC_RADIO8
#define BLENDMODERADIO IDC_RADIO9

#pragma endregion

#pragma region TerrainOptionControls
#define HEIGHTSCALE_EDIT IDC_EDIT1
#define CELLSPACING_EDIT IDC_EDIT8
#define CELLSPERPATCH_EDIT IDC_EDIT9
#define FOGSTART_EDIT IDC_EDIT10
#define FOGRANGE_EDIT IDC_EDIT17
#define FOGENABLE_CHECK IDC_CHECK1

#define HEIGHTMAPSIZE_COMBO IDC_COMBO1
#define HEIGHTMAP_BUTTON IDC_BUTTON8
#pragma endregion

#pragma region TesselationOptionControls
#define MINDIST_EDIT IDC_EDIT12
#define MAXDIST_EDIT IDC_EDIT18
#define MINTESS_EDIT IDC_EDIT19
#define MAXTESS_EDIT IDC_EDIT20
#pragma endregion

#define BUILDSHADOWMAPBUTTON_TERRAIN IDC_CHECK2
#define RECEIVESHADOWBUTTON_TERRAIN IDC_CHECK3

#define BRUSHRADIUS_EDIT IDC_EDIT21
#define BRUSHSHAPE_COMBO IDC_COMBO3
#define BRUSHSHAPE_BUTTON IDC_BUTTON9



INT_PTR CALLBACK TerrainRendererProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

class TerrainDialog : public ComponentDialog
{
public:
	static bool instantiated;
public:

public:
	TerrainDialog(HINSTANCE hInstance);
	~TerrainDialog();
public:
	bool OpenDialog(HWND hwnd);
	void Init(HWND hDlg);
	bool printMap();
public:
	AABB_MaxMin m_aabb;

private:
	//control ID에 해당하는 핸들, 수정해야될 값 포인터
	std::map<int, handleFloatPair> controlMap;
	std::pair<PWCHAR, PWCHAR> mapNames[6];

	//Material Map 행렬 EditBox
	HWND m_hDiffuseTileX;
	HWND m_hDiffuseTileY;
	HWND m_hDiffuseOffsetX;
	HWND m_hDiffuseOffsetY;
	
	HWND m_hColorR;
	HWND m_hColorG;
	HWND m_hColorB;
	HWND m_hColorA;
	
	HWND m_hBoundMinX;
	HWND m_hBoundMinY;
	HWND m_hBoundMinZ;
	HWND m_hBoundMaxX;
	HWND m_hBoundMaxY;
	HWND m_hBoundMaxZ;



private:
	TerrainRenderer* m_TerrainRenderer;
	std::vector<GeneralMaterial>* materials;
	Mesh* mesh;

private:
	void MapEditBoxUpdate(int materialIdx);
public:
	//메뉴눌렸을때 WM_COMMAND 
	virtual void MenuProc(HWND hDlg, WPARAM wParam) override;

	virtual void KeyDownProc(WPARAM wParam) override;

	virtual void MouseMoveProc(WPARAM btnState, LPARAM mousePos) override;

	virtual void CharProc(HWND hDlg, WPARAM wParam) override;

	void NotifyProc(HWND hDlg, LPARAM lParam);

	VOID WINAPI OnChildDialogInit(HWND hwndDlg);

private:
	// MsgProcedure을(를) 통해 상속됨
	virtual bool OpenDialog() override;//Tab의 자식 Dialog로 생성해야 하므로 사용하지 않는 함수

public:
	// ComponentDialog을(를) 통해 상속됨
	virtual bool UpdateView() override;


	// ComponentDialog을(를) 통해 상속됨
	virtual bool SetObject(GameObject * obj) override;

};




