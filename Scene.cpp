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

private:
	AssimpLoader objLoader;
	TextureMgr texMgr;
	EffectMgr effectMgr;
	ComponentMgr componentMgr;


private:
	Camera camera;
	DirectionalLight mDirLights[3];
	std::vector<GameObject> gameObjects;
		

private:
	float mTheta;
	float mPhi;
	float mRadius;
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
	: D3DApp(hInstance), 
	mTheta(1.3f*MathHelper::Pi), mPhi(0.4f*MathHelper::Pi), mRadius(2.5f)
{
	mMainWndCaption = L"Crate Demo";

	
	/*mDirLights[0].Ambient = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.6f, 0.6f, 0.6f, 16.0f);
	mDirLights[0].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[1].Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(1.4f, 1.4f, 1.4f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.3f, 0.3f, 0.3f, 16.0f);
	mDirLights[1].Direction = XMFLOAT3(-0.707f, 0.0f, 0.707f);*/

	mDirLights[0].Ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	mDirLights[0].Diffuse = XMFLOAT4(1.0f, 0.9f, 0.9f, 1.0f);
	mDirLights[0].Specular = XMFLOAT4(0.8f, 0.8f, 0.7f, 1.0f);
	mDirLights[0].Direction = XMFLOAT3(-0.57735f, -0.57735f, 0.57735f);

	mDirLights[1].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[1].Diffuse = XMFLOAT4(0.40f, 0.40f, 0.40f, 1.0f);
	mDirLights[1].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[1].Direction = XMFLOAT3(0.707f, -0.707f, 0.0f);

	mDirLights[2].Ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	mDirLights[2].Diffuse = XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f);
	mDirLights[2].Specular = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	mDirLights[2].Direction = XMFLOAT3(0.0f, 0.0, -1.0f);

	
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

	//카메라 초기화
	camera.SetPosition({ 50.0f, 0, 30.0f });
	camera.SetLens(0.5*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f); //수직시야각, 종횡비, 가까운평면, 먼평면
	camera.LookAt(camera.GetPosition(), { 0.0f,0.0f,0.0f }, { 0.0f,0.0f,-1 });
	
	
	// Must init Effects first since InputLayouts depend on shader signatures.
	Effects::InitAll(md3dDevice);
	InputLayouts::InitAll(md3dDevice);

	texMgr.Init(md3dDevice);
	effectMgr.Init(md3dDevice);
		
	return true;
}

void Scene::OnResize()
{
	D3DApp::OnResize();

	//종횡비가 바뀌었을 수 있으니 다시 설정
	camera.SetLens(0.5*MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
}

void Scene::UpdateScene(float dt)
{
	//
	// Control the camera.
	//
	if (GetAsyncKeyState('W') & 0x8000)
		camera.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		camera.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		camera.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		camera.Strafe(10.0f*dt);

	//// Convert Spherical to Cartesian coordinates.
	//float x = mRadius * sinf(mPhi)*cosf(mTheta);
	//float z = mRadius * sinf(mPhi)*sinf(mTheta);
	//float y = mRadius * cosf(mPhi);
		
}

void Scene::DrawScene()
{
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, reinterpret_cast<const float*>(&Colors::LightSteelBlue));
	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	md3dImmediateContext->IASetInputLayout(InputLayouts::Basic32);
	md3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		
	// Set per frame constants.
	effectMgr.SetPerFrame(mDirLights, nullptr, nullptr, camera.GetPosition());
	
		


	HR(mSwapChain->Present(0, 0));

	camera.UpdateViewMatrix();
}

void Scene::OnMouseDown(WPARAM btnState, int x, int y)
{
	SetCapture(mhMainWnd);
}

void Scene::OnMouseUp(WPARAM btnState, int x, int y)
{
	ReleaseCapture();
}

void Scene::OnMouseMove(WPARAM btnState, int x, int y)
{
	
}




