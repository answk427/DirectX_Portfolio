#include "LoadM3d.h"
 
bool M3DLoader::LoadM3d(const std::string& filename, 
						std::vector<Vertex::PosNormalTexTan>& vertices,
						std::vector<USHORT>& indices,
						std::vector<MeshGeometry::Subset>& subsets,
						std::vector<M3dMaterial>& mats)
{
	std::ifstream fin(filename);

	UINT numMaterials = 0;
	UINT numVertices  = 0;
	UINT numTriangles = 0;
	UINT numBones     = 0;
	UINT numAnimationClips = 0;

	std::string ignore;

	if( fin )
	{
		fin >> ignore; // file header text
		fin >> ignore >> numMaterials;
		fin >> ignore >> numVertices;
		fin >> ignore >> numTriangles;
		fin >> ignore >> numBones;
		fin >> ignore >> numAnimationClips;
 
		ReadMaterials(fin, numMaterials, mats);
		ReadSubsetTable(fin, numMaterials, subsets);
	    ReadVertices(fin, numVertices, vertices);
	    ReadTriangles(fin, numTriangles, indices);
 
		return true;
	 }
    return false;
}

bool M3DLoader::LoadM3d(const std::string& filename, 
						std::vector<Vertex::PosNormalTexTanSkinned>& vertices,
						std::vector<USHORT>& indices,
						std::vector<MeshGeometry::Subset>& subsets,
						std::vector<M3dMaterial>& mats,
						SkinnedData& skinInfo)
{
    std::ifstream fin(filename);

	UINT numMaterials = 0;
	UINT numVertices  = 0;
	UINT numTriangles = 0;
	UINT numBones     = 0;
	UINT numAnimationClips = 0;

	std::string ignore;

	if( fin )
	{
		fin >> ignore; // file header text
		fin >> ignore >> numMaterials;
		fin >> ignore >> numVertices;
		fin >> ignore >> numTriangles;
		fin >> ignore >> numBones;
		fin >> ignore >> numAnimationClips;
 
		std::vector<XMFLOAT4X4> boneOffsets;
		std::vector<int> boneIndexToParentIndex;
		std::map<std::string, AnimationClip> animations;

		ReadMaterials(fin, numMaterials, mats);
		ReadSubsetTable(fin, numMaterials, subsets);
	    ReadSkinnedVertices(fin, numVertices, vertices);
	    ReadTriangles(fin, numTriangles, indices);
		ReadBoneOffsets(fin, numBones, boneOffsets);
	    ReadBoneHierarchy(fin, numBones, boneIndexToParentIndex);
	    ReadAnimationClips(fin, numBones, numAnimationClips, animations);
 
		skinInfo.Set(boneIndexToParentIndex, boneOffsets, animations);

	    return true;


	}
    return false;
}

void M3DLoader::ReadMaterials(std::ifstream& fin, UINT numMaterials, std::vector<M3dMaterial>& mats)
{
	 std::string ignore;
     mats.resize(numMaterials);

	 std::string diffuseMapName;
	 std::string normalMapName;

     fin >> ignore; // materials header text
	 for(UINT i = 0; i < numMaterials; ++i)
	 {
			fin >> ignore >> mats[i].Mat.Ambient.x  >> mats[i].Mat.Ambient.y  >> mats[i].Mat.Ambient.z;
			fin >> ignore >> mats[i].Mat.Diffuse.x  >> mats[i].Mat.Diffuse.y  >> mats[i].Mat.Diffuse.z;
			fin >> ignore >> mats[i].Mat.Specular.x >> mats[i].Mat.Specular.y >> mats[i].Mat.Specular.z;
			fin >> ignore >> mats[i].Mat.Specular.w;
			fin >> ignore >> mats[i].Mat.Reflect.x >> mats[i].Mat.Reflect.y >> mats[i].Mat.Reflect.z;
			fin >> ignore >> mats[i].AlphaClip;
			fin >> ignore >> mats[i].EffectTypeName;
			fin >> ignore >> diffuseMapName;
			fin >> ignore >> normalMapName;

			mats[i].DiffuseMapName.resize(diffuseMapName.size(), ' ');
			mats[i].NormalMapName.resize(normalMapName.size(), ' ');
			std::copy(diffuseMapName.begin(), diffuseMapName.end(), mats[i].DiffuseMapName.begin());
			std::copy(normalMapName.begin(), normalMapName.end(), mats[i].NormalMapName.begin());
		}
}

void M3DLoader::ReadSubsetTable(std::ifstream& fin, UINT numSubsets, std::vector<MeshGeometry::Subset>& subsets)
{
    std::string ignore;
	subsets.resize(numSubsets);

	fin >> ignore; // subset header text
	for(UINT i = 0; i < numSubsets; ++i)
	{
        fin >> ignore >> subsets[i].Id;
		fin >> ignore >> subsets[i].VertexStart;
		fin >> ignore >> subsets[i].VertexCount;
		fin >> ignore >> subsets[i].FaceStart;
		fin >> ignore >> subsets[i].FaceCount;
    }
}

void M3DLoader::ReadVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex::PosNormalTexTan>& vertices)
{
	std::string ignore;
    vertices.resize(numVertices);

    fin >> ignore; // vertices header text
    for(UINT i = 0; i < numVertices; ++i)
    {
	    fin >> ignore >> vertices[i].Pos.x      >> vertices[i].Pos.y      >> vertices[i].Pos.z;
		fin >> ignore >> vertices[i].TangentU.x >> vertices[i].TangentU.y >> vertices[i].TangentU.z >> vertices[i].TangentU.w;
	    fin >> ignore >> vertices[i].Normal.x   >> vertices[i].Normal.y   >> vertices[i].Normal.z;
	    fin >> ignore >> vertices[i].Tex.x      >> vertices[i].Tex.y;
    }
}

void M3DLoader::ReadSkinnedVertices(std::ifstream& fin, UINT numVertices, std::vector<Vertex::PosNormalTexTanSkinned>& vertices)
{
	std::string ignore;
    vertices.resize(numVertices);

    fin >> ignore; // vertices header text
	int boneIndices[4];
	float weights[4];
    for(UINT i = 0; i < numVertices; ++i)
    {
	    fin >> ignore >> vertices[i].Pos.x        >> vertices[i].Pos.y          >> vertices[i].Pos.z;
		fin >> ignore >> vertices[i].TangentU.x   >> vertices[i].TangentU.y     >> vertices[i].TangentU.z >> vertices[i].TangentU.w;
	    fin >> ignore >> vertices[i].Normal.x     >> vertices[i].Normal.y       >> vertices[i].Normal.z;
	    fin >> ignore >> vertices[i].Tex.x        >> vertices[i].Tex.y;
		fin >> ignore >> weights[0]     >> weights[1]     >> weights[2]     >> weights[3];
		fin >> ignore >> boneIndices[0] >> boneIndices[1] >> boneIndices[2] >> boneIndices[3];

		vertices[i].Weights.x = weights[0];
		vertices[i].Weights.y = weights[1];
		vertices[i].Weights.z = weights[2];

		vertices[i].BoneIndices[0] = (BYTE)boneIndices[0]; 
		vertices[i].BoneIndices[1] = (BYTE)boneIndices[1]; 
		vertices[i].BoneIndices[2] = (BYTE)boneIndices[2]; 
		vertices[i].BoneIndices[3] = (BYTE)boneIndices[3]; 
    }
}

void M3DLoader::ReadTriangles(std::ifstream& fin, UINT numTriangles, std::vector<USHORT>& indices)
{
	std::string ignore;
    indices.resize(numTriangles*3);

    fin >> ignore; // triangles header text
    for(UINT i = 0; i < numTriangles; ++i)
    {
        fin >> indices[i*3+0] >> indices[i*3+1] >> indices[i*3+2];
    }
}
 
void M3DLoader::ReadBoneOffsets(std::ifstream& fin, UINT numBones, std::vector<XMFLOAT4X4>& boneOffsets)
{
	std::string ignore;
    boneOffsets.resize(numBones);

    fin >> ignore; // BoneOffsets header text
    for(UINT i = 0; i < numBones; ++i)
    {
        fin >> ignore >> 
            boneOffsets[i](0,0) >> boneOffsets[i](0,1) >> boneOffsets[i](0,2) >> boneOffsets[i](0,3) >>
            boneOffsets[i](1,0) >> boneOffsets[i](1,1) >> boneOffsets[i](1,2) >> boneOffsets[i](1,3) >>
            boneOffsets[i](2,0) >> boneOffsets[i](2,1) >> boneOffsets[i](2,2) >> boneOffsets[i](2,3) >>
            boneOffsets[i](3,0) >> boneOffsets[i](3,1) >> boneOffsets[i](3,2) >> boneOffsets[i](3,3);
    }
}

void M3DLoader::ReadBoneHierarchy(std::ifstream& fin, UINT numBones, std::vector<int>& boneIndexToParentIndex)
{
	std::string ignore;
    boneIndexToParentIndex.resize(numBones);

    fin >> ignore; // BoneHierarchy header text
	for(UINT i = 0; i < numBones; ++i)
	{
	    fin >> ignore >> boneIndexToParentIndex[i];
	}
}

void M3DLoader::ReadAnimationClips(std::ifstream& fin, UINT numBones, UINT numAnimationClips, 
								   std::map<std::string, AnimationClip>& animations)
{
	std::string ignore;
    fin >> ignore; // AnimationClips header text
    for(UINT clipIndex = 0; clipIndex < numAnimationClips; ++clipIndex)
    {
        std::string clipName;
        fin >> ignore >> clipName;
        fin >> ignore; // {

		AnimationClip clip;
		clip.BoneAnimations.resize(numBones);

        for(UINT boneIndex = 0; boneIndex < numBones; ++boneIndex)
        {
            ReadBoneKeyframes(fin, numBones, clip.BoneAnimations[boneIndex]);
        }
        fin >> ignore; // }

        animations[clipName] = clip;
    }
}

void M3DLoader::ReadBoneKeyframes(std::ifstream& fin, UINT numBones, BoneAnimation& boneAnimation)
{
	std::string ignore;
    UINT numKeyframes = 0;
    fin >> ignore >> ignore >> numKeyframes;
    fin >> ignore; // {

    boneAnimation.Keyframes.resize(numKeyframes);
    for(UINT i = 0; i < numKeyframes; ++i)
    {
        float t    = 0.0f;
        XMFLOAT3 p(0.0f, 0.0f, 0.0f);
        XMFLOAT3 s(1.0f, 1.0f, 1.0f);
        XMFLOAT4 q(0.0f, 0.0f, 0.0f, 1.0f);
        fin >> ignore >> t;
        fin >> ignore >> p.x >> p.y >> p.z;
        fin >> ignore >> s.x >> s.y >> s.z;
        fin >> ignore >> q.x >> q.y >> q.z >> q.w;

	    boneAnimation.Keyframes[i].TimePos      = t;
        boneAnimation.Keyframes[i].Translation  = p;
	    boneAnimation.Keyframes[i].Scale        = s;
	    boneAnimation.Keyframes[i].RotationQuat = q;
    }

    fin >> ignore; // }
}

//bool M3DLoader::LoadM3d(const std::string& filename,
//	std::vector<Vertex::PosNormalTexTanSkinned>& vertices,
//	std::vector<USHORT>& indices,
//	std::vector<MeshGeometry::Subset>& subsets,
//	std::vector<M3dMaterial>& mats,
//	SkinnedData& skinInfo)
//mCharacterModel = new SkinnedModel(md3dDevice, mTexMgr, "Models\\soldier.m3d", L"Textures\\");
void M3DLoader::convertVertex(const std::vector<Vertex::PosNormalTexTanSkinned>& vertexSrc,
	std::vector<MyVertex::BasicVertex>& vertexDest,
	std::vector<MyVertex::SkinnedData>& skinDest,
	const std::vector<USHORT>& indexSrc,
	std::vector<UINT>& indexDest,
	std::vector<MyVertex::Subset>& subsetDest,
	const std::vector<MeshGeometry::Subset>& subsetSrc,
	SkinnedData& skinSrc,
	Animator& destAnimator)
{
	//뼈구조 적재
	BoneDatas boneDatas;
	std::vector<int> destHierarchy;
	std::vector<XMFLOAT4X4> destOffsets;
	destHierarchy = skinSrc.GetHierarchy();
	destOffsets = skinSrc.GetOffsets();
	boneDatas.SetParents(destHierarchy);
	boneDatas.SetOffsets(destOffsets);

	destAnimator.SetBoneDatas(boneDatas);
	
	//애니메이션 적재
	auto& animations = skinSrc.GetAnimations();
	
	for (auto& elem : animations)
	{
		MyAnimationClip destClip;
		auto& clip = elem.second;
		destClip.m_clipName = elem.first;
		destClip.duration = clip.GetClipEndTime();
		
		
		auto& boneAnimations = clip.BoneAnimations;
		
		for (auto& bone : boneAnimations)
		{
			BoneFrames boneFrames(L"LoadM3d");
			std::vector<frameKey3> scales;
			std::vector<frameKey4> quaternions;
			std::vector<frameKey3> translates;

			for (auto& frame : bone.Keyframes)
			{
				frameKey3 scale;
				frameKey4 quaternion;
				frameKey3 translation;
				scale.first = frame.TimePos;
				scale.second = frame.Scale;
				quaternion.first = frame.TimePos;
				quaternion.second = frame.RotationQuat;
				translation.first = frame.TimePos;
				translation.second = frame.Translation;
				scales.push_back(scale);
				quaternions.push_back(quaternion);
				translates.push_back(translation);
			}
			boneFrames.InitScales(scales);
			boneFrames.InitQuaternions(quaternions);
			boneFrames.InitTranslations(translates);

			//Animation클립에 하나의 뼈 추가
			destClip.m_bones.push_back(boneFrames);
		}

		destAnimator.LoadAnimationClip(destClip);
		destAnimator.currClipName = destClip.m_clipName;
	}
	

	//skinnedVertex 적재
	vertexDest.resize(vertexSrc.size());
	skinDest.resize(vertexSrc.size());
		

	for (int i = 0; i < vertexSrc.size(); ++i)
	{
		vertexDest[i].pos = vertexSrc[i].Pos;
		vertexDest[i].tex = vertexSrc[i].Tex;
		vertexDest[i].normal = vertexSrc[i].Normal;
		for (int j = 0; j < 4; ++j)
		{
			skinDest[i].boneIndices[j] = vertexSrc[i].BoneIndices[j];
		}
		skinDest[i].weight = vertexSrc[i].Weights;	
	}

	//인덱스 적재
	indexDest.resize(indexSrc.size());
	for (int i = 0; i < indexSrc.size(); ++i)
	{
		indexDest[i] = indexSrc[i];
	}

	subsetDest.resize(subsetSrc.size());
	for (int i = 0; i < subsetSrc.size(); ++i)
	{
		subsetDest[i].IndexCount = subsetSrc[i].FaceCount*3;
		subsetDest[i].IndexStart = subsetSrc[i].FaceStart*3;
		subsetDest[i].VertexStart= subsetSrc[i].VertexStart;
		subsetDest[i].VertexCount= subsetSrc[i].VertexCount;
		subsetDest[i].materialNum= subsetSrc[i].Id;
	}
}

