#pragma once
#include <d3dUtil.h>
#include <algorithm>
#include <map>
#include <GameTimer.h>

//�ð�, �ش� Ű �������� ����
typedef std::pair<float, XMFLOAT3> frameKey3;
typedef std::pair<float, XMFLOAT4> frameKey4;

bool sortCompKey3(const frameKey3& first, const frameKey3& second);
bool sortCompKey4(const frameKey4& first, const frameKey4& second);
bool compKey3(const frameKey3& first, float compVal);
bool compKey4(const frameKey4& first, float compVal);

//�� �ϳ��� ���� ����� KeyFrame��
//�ִϸ��̼����� ������ ����� �θ�ȯ ��ķ� ����.
class BoneFrames
{
public:
	std::wstring m_boneName;
	BoneFrames(const std::wstring& name) : m_boneName(name) {}
private:
	std::vector<frameKey3> scaleKeys;
	std::vector<frameKey4> quaternionKeys;
	std::vector<frameKey3> translateKeys;
public:
	void Interpolate(float time, XMFLOAT4X4& dest);
	void InitScales(std::vector<frameKey3> scales);
	void InitQuaternions(std::vector<frameKey4> quaternions);
	void InitTranslations(std::vector<frameKey3> translations);
};


//AnimationManager�� ����� Ŭ����
class MyAnimationClip
{
public:
	double duration;
	std::string m_clipName; //animation clip�� �̸�
	std::vector<BoneFrames> m_bones;

	void Interpolate(float time, std::vector<XMFLOAT4X4>& toParents);
};



struct BoneDatas
{
	//Offset ���
	std::vector<XMFLOAT4X4> offsets;
	//Bone ��������
	std::vector<int> m_parentIndices;
	//�� ���뿡�� Root�� ���� ���
	std::vector<XMFLOAT4X4> toRoots;
	//�� ������ ���� ��ȯ
	std::vector<XMFLOAT4X4> m_finalTransforms;
	//�θ� ������� ��ȯ
	std::vector<XMFLOAT4X4> m_toParentMatrix;

	void SetOffsets(const std::vector<XMFLOAT4X4>& offsetVec);
	void SetParents(const std::vector<int>& parents);
	void SetParentMatrix(const std::vector<XMFLOAT4X4>& parentMats);
};


class Animator
{
public:
	Animator() : timePos(0.0f)  {}
	Animator(const Animator& other);
	Animator& operator=(const Animator& other);
	//�ִϸ����Ϳ��� �����ϴ� �� ����
	BoneDatas boneDatas;
		
	//���� �����ϴ� Ŭ���̸�
	std::string currClipName;
	//���� �������� �ð���ġ
	float timePos;
	//���� �ִϸ����Ϳ��� ������ �� �ִ� Ŭ����
	std::map<std::string, MyAnimationClip> clips;
	
	
public:
	void SetBoneDatas(BoneDatas& data) { boneDatas = data; }
	void LoadAnimationClip(MyAnimationClip& clip);
	void Update(float deltaTime);

public:
	void ChangeClip(const std::string& clipName);
};