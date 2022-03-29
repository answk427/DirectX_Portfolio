#include "MeshReaderWriter.h"

bool MeshReaderWriter::ReadFile(const FR_fileName& fileName, char* dest)
{
	std::ifstream inFile(fileName, std::ios::binary);
	
	if (!inFile.is_open())
		return false;

	//*******header 정보읽기 시작**********//

	UINT idLength,vertexSize, indexSize, subsetSize;
	char str[255];
	
	//id 길이 읽기
	inFile.read((char*)(&idLength), sizeof(UINT));
	
	//id 읽기
	inFile.read(str, idLength);
	
	//size 읽기
	inFile.read((char*)(&vertexSize), sizeof(UINT));
	inFile.read((char*)(&indexSize), sizeof(UINT));
	inFile.read((char*)(&subsetSize), sizeof(UINT));
	//*******header 정보읽기 끝**********//
	
	Mesh* meshData = (Mesh*)dest;
	
	//id 설정
	meshData->SetID(str);
	//임시 정점배열
	std::vector<MyVertex::BasicVertex> vertexSrc(vertexSize);
	//임시 인덱스배열
	std::vector<UINT> indexSrc(indexSize);
	//임시 Subset 배열
	std::vector<Subset> subsetSrc(subsetSize);

	//정점 구조체 읽기
	inFile.read((char*)(&vertexSrc[0]), sizeof(MyVertex::BasicVertex)*vertexSize);
	
	//인덱스 읽기
	inFile.read((char*)(&indexSrc[0]), sizeof(UINT) * indexSize);

	//Subset 읽기
	inFile.read((char*)(&subsetSrc[0]), sizeof(Subset) * subsetSize);

	meshData->Init(vertexSrc, indexSrc, subsetSrc);
	
	return true;
}

MeshReaderWriter& MeshReaderWriter::Instance()
{
	static MeshReaderWriter* meshReaderWriter = new MeshReaderWriter();
	return *meshReaderWriter;
}


bool MeshReaderWriter::WriteFile(void * data)
{
	
	Mesh* meshData = (Mesh*)data;
	
	//mesh id에 확장자를 붙여 파일이름으로 사용
	std::string fileName=  meshData->id + "." + fileType;
	std::wstring fileFullName = fileDirectory + L"/" + ConvertWSTR(fileName);

	//파일이 존재하면 false 반환
	
	if (FileExist(fileFullName, std::ios::binary))
		return false;

	std::ofstream outFile;
	//outFile.open(fileName.c_str(), std::ios::binary);
	outFile.open(fileFullName.c_str(), std::ios::binary);

	if (!outFile.is_open())
		return false;

	//정점배열
	const std::vector<MyVertex::BasicVertex>& vertexSrc = meshData->GetVertices();
	//인덱스배열
	const std::vector<UINT> indexSrc = meshData->GetIndices();
	//Subset 배열
	const std::vector<Subset> subsetSrc = meshData->GetSubsets();
	
	

	

	//*******header에 정보쓰기**********
	//Mesh의 id길이,id 쓰기
	UINT idLength = meshData->id.length() + 1;
	outFile.write((const char*)(&idLength), sizeof(UINT));
	outFile.write(meshData->id.c_str(), idLength);
	//정점 개수 쓰기
	UINT vertexSize = vertexSrc.size();
	outFile.write((const char*)(&vertexSize), sizeof(size_t));
	//인덱스 개수 쓰기
	UINT indexSize = indexSrc.size();
	outFile.write((const char*)(&indexSize), sizeof(indexSize));
	//Subset 개수 쓰기
	UINT subsetSize = subsetSrc.size();
	outFile.write((const char*)(&subsetSize), sizeof(subsetSize));
	//*******header에 정보쓰기**********


	//정점 구조체 쓰기
	for (int i = 0; i < vertexSize; i++)
	{
		outFile.write((const char*)(&vertexSrc[i]), sizeof(MyVertex::BasicVertex));
	}
	//인덱스 쓰기
	for (int i = 0; i < indexSize; i++)
	{
		outFile.write((const char*)(&indexSrc[i]), sizeof(UINT));
	}
	//Subset 쓰기
	for (int i = 0; i < subsetSize; i++)
	{
		outFile.write((const char*)(&subsetSrc[i]), sizeof(Subset));
	}

	outFile.close();


	return true;
}