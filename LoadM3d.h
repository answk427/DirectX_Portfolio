#ifndef LOADM3D_H
#define LOADM3D_H

#include "MeshGeometry.h"
#include "LightHelper.h"
#include "SkinnedData.h"
#include "Vertex.h"
#include "BufferResource.h"
#include "AnimationClass.h"

struct M3dMaterial
{
	Material Mat;
	bool AlphaClip;
	std::string EffectTypeName;
	std::wstring DiffuseMapName;
	std::wstring NormalMapName;
};

class M3DLoader
{
public:
	bool LoadM3d(const std::string& filename, 
		std::vector<Vertex::PosNormalTexTan>& vertices,
		std::vector<USHORT>& indices,
		std::vector<MeshGeometry::Subset>& subsets,
		std::vector<M3dMaterial>& mats);
	bool LoadM3d(const std::string& filename, 
		std::vector<Vertex::PosNormalTexTanSkinned>& vertices,
		std::vector<USHORT>& indices,
		std::vector<MeshGeometry::Subset>& subsets,
		std::vector<M3dMaterial>& mats,
		SkinnedData& skinInfo);

	void convertVertex(const std::vector<Vertex::PosNormalTexTanSkinned>& vertexSrc,
		std::vector<MyVertex::BasicVertex>& vertexDest,
		std::vector<MyVertex::SkinnedData>& skinDest,
		const std::vector<USHORT>& indexSrc,
		std::vector<UINT>& indexDest,
		std::vector<MyVertex::Subset>& subsetDest,
		const std::vector<MeshGeometry::Subset>& subsetSrc,
		SkinnedData& skinSrc,
		Animator& destAnimator
		);
private:
	void ReadMaterials(std::ifstream& fin, UINT numMaterials, std::vector<M3dMaterial>& mats);
	void ReadSubsetTable(std::ifstream& fin, UINT numSubsets, std::vector<MeshGeometry::Subset>& subsets);
	void ReadVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex::PosNormalTexTan>& vertices);
	void ReadSkinnedVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex::PosNormalTexTanSkinned>& vertices);
	void ReadTriangles(std::ifstream& fin, UINT numTriangles, std::vector<USHORT>& indices);
	void ReadBoneOffsets(std::ifstream& fin, UINT numBones, std::vector<XMFLOAT4X4>& boneOffsets);
	void ReadBoneHierarchy(std::ifstream& fin, UINT numBones, std::vector<int>& boneIndexToParentIndex);
	void ReadAnimationClips(std::ifstream& fin, UINT numBones, UINT numAnimationClips, std::map<std::string, AnimationClip>& animations);
	void ReadBoneKeyframes(std::ifstream& fin, UINT numBones, BoneAnimation& boneAnimation);
	
	
};



#endif // LOADM3D_H