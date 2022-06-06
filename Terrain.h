#pragma once
#include "Renderer.h"
#include "Mouse.h"
#include "RayPicking.h"

#define RAISEDELTA 0.02

class TerrainRenderer : public Renderer
{
public:
	class TerrainInitInfo
	{
	public:
		TerrainInitInfo() : HeightScale(200.0f), HeightmapWidth(2049), HeightmapHeight(2049), CellSpacing(0.5f), layerFilenames(5, L"NULL")
		{}
		TerrainInitInfo(const TerrainInitInfo& other)
		{
			layerFilenames = other.layerFilenames;
			HeightMapFilename = other.HeightMapFilename;
			BlendMapFilename = other.BlendMapFilename;
			HeightScale = other.HeightScale;
			HeightmapWidth = other.HeightmapWidth;
			CellSpacing = other.CellSpacing;
		}
		TerrainInitInfo& operator=(const TerrainInitInfo& other)
		{
			layerFilenames = other.layerFilenames;
			HeightMapFilename = other.HeightMapFilename;
			BlendMapFilename = other.BlendMapFilename;
			HeightScale = other.HeightScale;
			HeightmapWidth = other.HeightmapWidth;
			CellSpacing = other.CellSpacing;

			return *this;
		}
		void testfunc() { return; }

		std::wstring HeightMapFilename;
		//std::wstring BasicMapFilename;
		//std::wstring LayerMapFilename1;
		//std::wstring LayerMapFilename2;
		//std::wstring LayerMapFilename3;
		//std::wstring LayerMapFilename4;
		std::vector<std::wstring> layerFilenames;

		std::wstring BlendMapFilename;
		float HeightScale;
		UINT HeightmapWidth;
		UINT HeightmapHeight;
		float CellSpacing;
	};

private:
	std::vector<MyVertex::Terrain> m_patchVertices;
	std::vector<XNA::AxisAlignedBox> m_aabb;

public:
	TerrainInitInfo m_terrainData;
	TesselationData m_tesselationData;
	std::unique_ptr<BrushDesc> m_brush;
	static std::vector<int> HeightMapSizes;
	Camera* m_camera;
public:
	TerrainRenderer(const componentID& id) : Renderer(id, ComponentType::TERRAIN), mtextureArraySRV(0), mblendTextureSRV(0), mHeightMapSRV(0), m_enableFog(0),
		m_fogStart(15.0f), m_fogRange(175.0f), CellsPerPatch(64), m_brush(std::make_unique<BrushDesc>()), m_camera(0), m_hmapTex(0), m_blendMapTex(0), m_selectedMap(0)
	{
		mesh = new Mesh("TerrainRendererMesh");
		mesh->Init(std::vector<MyVertex::BasicVertex>(), std::vector<UINT>());
		InitEffects();
	}
	TerrainRenderer(const componentID& id, const TerrainInitInfo& info) : TerrainRenderer(id)
	{
		m_terrainData = info;
	}
	TerrainRenderer(const TerrainRenderer& other) : TerrainRenderer(other.id, other.m_terrainData) {}

	TerrainRenderer& operator=(const TerrainRenderer& other)
	{
		m_terrainData = other.m_terrainData;
		m_tesselationData = other.m_tesselationData;
		m_fogStart = other.m_fogStart;
		m_fogRange = other.m_fogRange;
		m_enableFog = other.m_enableFog;
		CellsPerPatch = other.CellsPerPatch;
		m_camera = other.m_camera;
		Init();

		return *this;
	}


	~TerrainRenderer()
	{
		delete mesh;
		ReleaseCOM(mtextureArraySRV);
		ReleaseCOM(mblendTextureSRV);
		ReleaseCOM(mHeightMapSRV);
		ReleaseCOM(m_hmapTex);
		ReleaseCOM(m_blendMapTex);
	}


private:
	ID3D11ShaderResourceView* mtextureArraySRV; //기본 텍스쳐, layer 텍스쳐 합친 배열
	ID3D11ShaderResourceView* mblendTextureSRV;
	ID3D11ShaderResourceView* mHeightMapSRV;
	ID3D11Texture2D* m_hmapTex;
	ID3D11Texture2D* m_blendMapTex;
	

	std::vector<float> mHeightmap; //높이맵을 읽어와 저장할 vector
	std::vector<unsigned char> mHeightmapDatas;

	bool m_enableFog;
	float m_fogStart;
	float m_fogRange;

	UINT m_selectedMap;
		

public: //Renderer 함수 재정의
	virtual void InitEffects() override;
	virtual void Draw(ID3D11DeviceContext* context, Camera* camera) override;
	virtual void Init() override;
	virtual void Update() override;

public:
	void SetCamera(Camera* camera) { m_camera = camera; }

	float GetWidth()const;
	float GetDepth()const;
	float GetHeight(float x, float z)const;
	float GetTangent(float posX, float posZ, float x, float z)const;
	bool GetLocalPosition(float posX, float posZ, float* destX, float* destZ);
	void LocalToWorld(float x, float z, float* destX, float* destZ, float halfWidth, float halfDepth);
	//높이맵의 높이를 높이는 함수
	void RaiseHeight(float x, float z);
	//블랜드맵을 수정하는 함수
	void ModifyBlendMap(float x, float z);


	//반직선과 terrain과의 교차점을 구하는 함수
	bool GetRayIntersectPos(XMVECTOR& rayOrigin, XMVECTOR& rayDir, XMVECTOR* destPos);
	//스크린 좌표에서 Terrain의 교차점을 구하는 함수
	bool GetScreenIntersect();
	

	bool GetEnableFog() { return m_enableFog; }
	void SetEnableFog(bool b)
	{ m_enableFog = b;
		//RaiseHeight(m_brush->centerW.x, m_brush->centerW.z);	
		//ModifyBlendMap(m_brush->centerW.x, m_brush->centerW.z);
	}
	float GetFogStart() { return m_fogStart; }
	void SetFogStart(float fogStart) { m_fogStart = fogStart; }
	float GetFogRange() { return m_fogRange; }
	void SetFogRange(float fogRange) { m_fogRange = fogRange; }

	void SetHeightScale(float height);

	void SetHeightMap(std::wstring& filePath);
	void SetLayerMap(std::wstring& filePath, UINT i);
	void SetBlendMap(std::wstring& filePath);

	void SetCellsPerPatch(UINT cells)
	{
		if (CellsPerPatch == cells || cells == 0)
			return;
		CellsPerPatch = cells;
		Init();
	}
	int GetCellsPerPatch() { return CellsPerPatch; }

	void SetCellSpacing(float spacing)
	{
		if (spacing <= 0 || m_terrainData.CellSpacing == spacing)
			return;
		m_terrainData.CellSpacing = spacing;
		BuildQuadPatchVB(m_texMgr.md3dDevice);
	}
	float GetCellSpacing() { return m_terrainData.CellSpacing; }
	
	//BlendingMap 수정을 위해 layer맵 중 하나를 선택하는 함수
	void SelectLayerMap(UINT num) { m_selectedMap = num; }

private:
	void LoadHeightmap(); //높이맵 read

	//평활화 관련 함수
	void Smooth();
	bool InBounds(int i, int j);
	float Average(int i, int j);


	void CalcAllPatchBoundsY();
	void CalcPatchBoundsY(UINT i, UINT j);
	void BuildQuadPatchVB(ID3D11Device* device);
	void BuildQuadPatchIB(ID3D11Device* device);
	void BuildHeightmapSRV(ID3D11Device* device);
	void BuildLayermapSRV();
	void BuildBlendmapSRV();
	

private:

	int CellsPerPatch = 64; //각 패치마다 몇칸으로 나눌지 정하는 변수
	std::vector<XMFLOAT2> mPatchBoundsY;

	ID3D11Buffer* mQuadPatchVB;
	ID3D11Buffer* mQuadPatchIB;

	UINT mNumPatchVertices;
	UINT mNumPatchQuadFaces;

	UINT mNumPatchVertRows;
	UINT mNumPatchVertCols;

};

