#pragma once
#include <d3dUtil.h>
#include <algorithm>
#include <map>

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
class AnimationClip
{
public:
	double duration;
	double ticksPerSecond;
	std::string m_clipName; //animation clip�� �̸�
	std::vector<BoneFrames> m_bones;
	
};

typedef std::vector<BoneFrames*> AnimatorBones;

class AnimatorClip
{
public:
	AnimatorClip(){ XMStoreFloat4x4(&identity, XMMatrixIdentity()); }
public:
	double duration;
	double ticksPerSecond;
	std::string m_clipName;
	AnimatorBones m_bones;
	XMFLOAT4X4 identity;
public:
	void Interpolate(float time, std::vector<XMFLOAT4X4>& toParents);
	//���� �� Ŭ���� ����ϴ� ��� ������ ���� m_bones�� �ʱ�ȭ
	void InitHierarchy(std::map<std::wstring, int>& m_nodeNameIdx, AnimationClip& clip);
};



class Animator
{
public:
	Animator() {}
	std::map<std::wstring, int>& m_nodeNameIdx;
	std::vector<int>& m_parentIndices;
	//Root�� ���� ���
	std::vector<XMFLOAT4X4> toRoots; 
	//Offset ���
	std::vector<XMFLOAT4X4> offsets;
	//���� �ִϸ����Ϳ��� ������ �� �ִ� Ŭ����
	std::map<std::string, AnimatorClip> clips;
	//���� �����ϴ� Ŭ���̸�
	std::string currClipName;
public:
	void AddNode(std::wstring* parentName, std::wstring* childName, XMFLOAT4X4& offsetMat);
	void LoadAnimationClip(AnimationClip& clip);
	void Update(float time);
};