//***************************************************************************************
// CrateDemo.cpp by Frank Luna (C) 2011 All Rights Reserved.
//
// Demonstrates texturing a box.
//
// Controls:
//		Hold the left mouse button down and move the mouse to rotate.
//      Hold the right mouse button down to zoom in and out.
//
//***************************************************************************************
//

#include "d3dApp.h"
#include "d3dx11Effect.h"
#include "GeometryGenerator.h"
#include "MathHelper.h"
#include "LightHelper.h"
#include "Effects.h"
#include "Vertex.h"
#include "AssimpLoader.h"
#include "Renderer.h"
#include <Camera.h>
#include "GameObject.h"
#include "ComponentMgr.h"
#include "ObjectMgr.h"
#include "MeshMgr.h"
#include "HierarchyDialog.h"
#include "DataManager.h"
#include "ObjectCommand.h"
#include "BoundingBoxRenderer.h"
#include "Culling.h"
#include "RayPicking.h"
#include "TreeBillBoardRenderer.h"
#include "Octree.h"
#include "SimpleLineRenderer.h"
#include <random>


class Scene : public D3DApp
{
public:
	Scene(HINSTANCE hInstance);
	~Scene();

	bool Init();
	void OnResize();
	void UpdateScene(float dt);
	void DrawScene();

	void OnMouseDown(WPARAM btnState, int x, int y);
	void OnMouseUp(WPARAM btnState, int x, int y);
	void OnMouseMove(WPARAM btnState, int x, int y);

	void MenuProc(HWND hDlg, WPARAM wParam) override;

private://�������
	HierarchyDialog* m_HierarchyDialog;
	AssimpLoader objLoader;
	BoundingBoxRenderer* m_boundingBoxRenderer;
	SimpleLineRenderer* m_OctreeRenderer;
	TreeBillBoardRenderer* m_treeBillBoardRenderer;
	Frustum* m_Frustum;
	Octree* m_Octree;
	std::vector<unique_ptr<Renderer>> boxes;
	
private:
	DataManager& dataMgr;
	TextureMgr& texMgr;
	EffectMgr& effectMgr;
	ComponentMgr& componentMgr;
	ObjectMgr& objectMgr;
	MeshMgr meshMgr;

private:
	Camera camera;
	//DirectionalLight mDirLights[3];
	std::vector<DirectionalLight> mDirLights;
	std::vector<GameObject> gameObjects;


private:
	POINT mLastMousePos;

	float mTheta;
	float mPhi;
	float mRadius;
	
	D3D11_VIEWPORT m_currentViewPort;
	//�� �����Ӹ��� ������ �� Renderer ������Ʈ���� ����
	std::vector<Renderer*> m_drawableRenderers;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	Scene theApp(hInstance);

	if (!theApp.Init())
		return 0;	

	return theApp.Run();
}




Scene::Scene(HINSTANCE hInstance)
	: D3DApp(hInstance), meshMgr(md3dDevice), componentMgr(ComponentMgr::Instance()),
	mTheta(1.3f*MathHelper::Pi),
	mPhi(0.4f*MathHelper::Pi), mRadius(2.5f),
	m_HierarchyDialog(new HierarchyDialog(hInstance)),
	texMgr(TextureMgr::Instance()), effectMgr(EffectMgr::Instance()),
	dataMgr(DataManager::Instance()), objectMgr(ObjectMgr::Instance())
	, m_boundingBoxRenderer(0), m_treeBillBoardRenderer(0)
{
	objectMgr.Init(&meshMgr, &componentMgr);
	mMainWndCaption = L"Crate Demo";

	mLastMousePos.x = 0;
	mLastMousePos.y = 0;

}

Scene::~Scene()
{
	Effects::DestroyAll();
	InputLayouts::DestroyAll();
}

bool Scene::Init()
{
	if (!D3DApp::Init())
		return false;
		

	//test
	meshMgr.Init(md3dDevice);
	texMgr.Init(md3dDevice, md3dImmediateContext);
	effectMgr.Init(md3dDevice);
	dataMgr.Init();
	
	//renderer �޸��Ҵ�
	m_boundingBoxRenderer = new BoundingBoxRenderer(md3dDevice, md3dImmediateContext);
	m_OctreeRenderer = new SimpleLineRenderer(md3dDevice, md3dImmediateContext);
	m_treeBillBoardRenderer = new TreeBillBoardRenderer();
	
	//ī�޶� �ʱ�ȭ
	camera.SetPosition({ 0.0f, 0.0f, -70.0f });
	camera.SetLens(0.5*MathHelper::Pi, AspectRatio(), 1.0f, 2000.0f); //�����þ߰�, ��Ⱦ��, ��������, �����
	camera.LookAt(camera.GetPosition(), { 0.0f,0.0f,0.0f }, { 0.0f,1.0f,0.0f });

	//Frustum Class ����
	m_Frustum = new Frustum(&camera);
	m_Octree = new Octree(m_Frustum, m_OctreeRenderer);
	OctreeCommand::Init(m_Octree);

	// Must init Effects first since InputLayouts depend on shader signatures.
	//Effects::InitAll(md3dDevice);
	dataMgr.LoadEffectData();
	//InputLayouts::InitAll(md3dDevice);





	//Hierarchy �ʱ�ȭ
	m_HierarchyDialog->Init(mhMainWnd, m_boundingBoxRenderer);
	m_HierarchyDialog->OpenDialog(mhMainWnd);
	
	/*Lighting* l = dynamic_cast<Lighting*>(componentMgr.CreateComponent(ComponentType::LIGHT));
	XMFLOAT4 a(0.5f, 0.5f, 0.5f, 1.0f);
	XMFLOAT3 b(1.0f, 1.0f, 1.0f);
	l->SetBasic(a, a, a);
	l->SetAtt(b);
	l->SetDirection(b);

	Lighting* l2 = dynamic_cast<Lighting*>(componentMgr.CreateComponent(ComponentType::LIGHT));
	b = { -1.0f,-1.0f,-1.0f};
	l2->SetBasic(a, a, a);
	l2->SetAtt(b);
	l2->SetDirection(b);

	Lighting* l3 = dynamic_cast<Lighting*>(componentMgr.CreateComponent(ComponentType::LIGHT));
	b = { -0.5f,0.5f,-0.5f };
	l3->SetBasic(a, a, a);
	l3->SetAtt(b);
	l3->SetDirection(b);


	Lighting* l4 = dynamic_cast<Lighting*>(componentMgr.CreateComponent(ComponentType::LIGHT));
	b = { +0.5f,-0.5f,0.5f };
	l4->SetBasic(a, a, a);
	l4->SetAtt(b);
	l4->SetDirection(b);*/

	SetFocus(mhMainWnd);

	//test
	// �õ尪�� ��� ���� random_device ����.
	std::random_device rd;

	// random_device �� ���� ���� ���� ������ �ʱ�ȭ �Ѵ�.
	std::mt19937 gen(rd());

	// 0 ���� 99 ���� �յ��ϰ� ��Ÿ���� �������� �����ϱ� ���� �յ� ���� ����.
	std::uniform_int_distribution<int> dis(-100, 100);
		
	/*GeometryGenerator geo;
	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			for (int k = 0; k < 10; ++k)
			{
				Mesh* mesh = new Mesh();
				geo.CreateBox(4, 4, 4, *mesh);
				mesh->InitVB(md3dDevice);
				mesh->InitIB(md3dDevice);
				Transform* tr = new Transform(nullptr);
				tr->SetPosition(i * dis(gen), j * dis(gen), k * dis(gen));
				tr->UpdateWorld();
				unique_ptr<Renderer> box = make_unique<Renderer>("tempId"+to_string(i+j*10+k*100), ComponentType::MESHRENDERER, mesh);
				box->SetTransform(tr);
				boxes.push_back(std::move(box));
				CommandQueue::AddCommand(new OctreeAddObject(boxes.back().get()));
			}
		}
	}*/


	return true;
}

void Scene::OnResize()
{
	D3DApp::OnResize();

	//��Ⱦ�� �ٲ���� �� ������ �ٽ� ����
	camera.SetLens(0.5*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	//viewport ������ ������.
	UINT nums[1] = { 1 };
	md3dImmediateContext->RSGetViewports(nums, &m_currentViewPort);
}

void Scene::UpdateScene(float dt)
{

	//
	// Control the camera.
	//


	//���� 10�̾���
	if (GetAsyncKeyState('W') & 0x8000)
		camera.Walk(50.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		camera.Walk(-50.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		camera.Strafe(-50.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		camera.Strafe(50.0f*dt);

	//// Convert Spherical to Cartesian coordinates.
	//float x = mRadius * sinf(mPhi)*cosf(mTheta);
	//float z = mRadius * sinf(mPhi)*sinf(mTheta);
	//float y = mRadius * cosf(mPhi);

	CommandQueue::AllExecute();

	//��� ������Ʈ ������Ʈ
	componentMgr.Update();
	//���� UI ������Ʈ
	m_HierarchyDialog->Update();
	//ī�޶� ��ġ�� ���� Frustum ������Ʈ
	camera.UpdateViewMatrix();
	m_Frustum->Update();
	

	//����ü ����
	//���� ��������ϵ��� �����´�.
	m_drawableRenderers = componentMgr.GetDrawableRenderers();

	for (auto elem : m_drawableRenderers)
	{
		int cullingResult = FrustumCulling::ComputeFrustumCulling(&elem->GetMesh()->GetAABB(),
			&camera,
			&XMLoadFloat4x4(&elem->GetTransform()->m_world));

		if (cullingResult != 0)
			elem->InstancingUpdate();
	}
	
}

void Scene::DrawScene()
{
	

	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);


	effectMgr.SetPerFrame(componentMgr.getLightings(), camera.GetPosition());

	//Rendering
	
	//�ٿ���ڽ� ������
	m_boundingBoxRenderer->Draw(md3dImmediateContext, &camera);
	//Octree ������
	m_OctreeRenderer->Draw(md3dImmediateContext, &camera);
	//treeBillBoard ������
	m_treeBillBoardRenderer->Draw(md3dImmediateContext, &camera);
	
	componentMgr.Render(md3dImmediateContext, &camera);
	
	m_Octree->Render(md3dImmediateContext);

	

	HR(mSwapChain->Present(0, 0));

	
}

void Scene::OnMouseDown(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		mLastMousePos.x = x;
		mLastMousePos.y = y;
		SetCapture(mhMainWnd);

		//������������ ������Ʈ�� ����
		Renderer* picked = RayPicking::NearestOfIntersectRayAABB(&m_currentViewPort, m_drawableRenderers, &camera, x, y);
		if (picked != nullptr)
		{
			GameObject* pickedObj = reinterpret_cast<GameObject*>(picked->GetTransform()->m_owner_obj);
			m_boundingBoxRenderer->SetObject(pickedObj);
			m_HierarchyDialog->SelectObject(pickedObj->GetID());
		}
			
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		//
	}
		
	
}

void Scene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Scene::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (btnState & MK_LBUTTON)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - mLastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - mLastMousePos.y));

		camera.Pitch(dy);
		camera.RotateY(dx);
	}

	mLastMousePos.x = x;
	mLastMousePos.y = y;
}



void Scene::MenuProc(HWND hDlg, WPARAM wParam)
{
	int wmId = LOWORD(wParam);
	static WCHAR title[MAX_PATH];
	static WCHAR full_path[MAX_PATH];

	switch (wmId)
	{
	case ID_40001: //����
	{
		std::vector<LPCWSTR> extensions = { L"jpg",L"bmp" };
		bool success = dataMgr.FileOpen(hDlg, title, full_path, extensions);
		break;
	}
	case ID_40002: //����

		MessageBox(0, L"����", L"����", MB_OK);
		break;

	case ID_40007: //Import FBX
	{
		USES_CONVERSION;
		std::vector<LPCWSTR> extensions = { L"fbx" };
		bool success = dataMgr.FileOpen(hDlg, title, full_path, extensions);
		if (success)
		{
			GameObject* gameObj = objectMgr.CreateObjectFromFile(W2A(full_path));
			m_HierarchyDialog->TreeInsertObject(gameObj);
		}


		break;
	}
	case ID_40010: //Import mesh
	{
		std::vector<LPCWSTR> extensions = { L"mesh" };
		bool success = dataMgr.FileOpen(hDlg, title, full_path, extensions);
		if(success)
			meshMgr.CreateMeshFromFile(full_path);
		break;
	}
	case ID_GAMEOBJECT_EMPTYOBJECT:
	{
		CommandQueue::AddCommand(new CreateEmptyObject(*m_HierarchyDialog));
		break;
	}
	case ID_GAMEOBJECT_BOX:
	{
		GameObject& gameObj = objectMgr.CreateBasicBoxObject();
		m_HierarchyDialog->TreeInsertObject(&gameObj);
		break;
	}
	case ID_GAMEOBJECT_GRID:
	{
		GameObject& gameObj = objectMgr.CreateBasicGrid();
		m_HierarchyDialog->TreeInsertObject(&gameObj);
		break;
	}

	case ID_40004: // Ctrl+Z
		MessageBox(mhMainWnd, L"Undo", L"Undo", MB_OK);
		CommandQueue::Undo();
		break;

	}
}