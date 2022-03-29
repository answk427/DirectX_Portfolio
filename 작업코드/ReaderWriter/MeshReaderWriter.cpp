#include "MeshReaderWriter.h"

bool MeshReaderWriter::ReadFile(const FR_fileName& fileName, char* dest)
{
	std::ifstream inFile(fileName, std::ios::binary);
	
	if (!inFile.is_open())
		return false;

	//*******header �����б� ����**********//

	UINT idLength,vertexSize, indexSize, subsetSize;
	char str[255];
	
	//id ���� �б�
	inFile.read((char*)(&idLength), sizeof(UINT));
	
	//id �б�
	inFile.read(str, idLength);
	
	//size �б�
	inFile.read((char*)(&vertexSize), sizeof(UINT));
	inFile.read((char*)(&indexSize), sizeof(UINT));
	inFile.read((char*)(&subsetSize), sizeof(UINT));
	//*******header �����б� ��**********//
	
	Mesh* meshData = (Mesh*)dest;
	
	//id ����
	meshData->SetID(str);
	//�ӽ� �����迭
	std::vector<MyVertex::BasicVertex> vertexSrc(vertexSize);
	//�ӽ� �ε����迭
	std::vector<UINT> indexSrc(indexSize);
	//�ӽ� Subset �迭
	std::vector<Subset> subsetSrc(subsetSize);

	//���� ����ü �б�
	inFile.read((char*)(&vertexSrc[0]), sizeof(MyVertex::BasicVertex)*vertexSize);
	
	//�ε��� �б�
	inFile.read((char*)(&indexSrc[0]), sizeof(UINT) * indexSize);

	//Subset �б�
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
	
	//mesh id�� Ȯ���ڸ� �ٿ� �����̸����� ���
	std::string fileName=  meshData->id + "." + fileType;
	std::wstring fileFullName = fileDirectory + L"/" + ConvertWSTR(fileName);

	//������ �����ϸ� false ��ȯ
	
	if (FileExist(fileFullName, std::ios::binary))
		return false;

	std::ofstream outFile;
	//outFile.open(fileName.c_str(), std::ios::binary);
	outFile.open(fileFullName.c_str(), std::ios::binary);

	if (!outFile.is_open())
		return false;

	//�����迭
	const std::vector<MyVertex::BasicVertex>& vertexSrc = meshData->GetVertices();
	//�ε����迭
	const std::vector<UINT> indexSrc = meshData->GetIndices();
	//Subset �迭
	const std::vector<Subset> subsetSrc = meshData->GetSubsets();
	
	

	

	//*******header�� ��������**********
	//Mesh�� id����,id ����
	UINT idLength = meshData->id.length() + 1;
	outFile.write((const char*)(&idLength), sizeof(UINT));
	outFile.write(meshData->id.c_str(), idLength);
	//���� ���� ����
	UINT vertexSize = vertexSrc.size();
	outFile.write((const char*)(&vertexSize), sizeof(size_t));
	//�ε��� ���� ����
	UINT indexSize = indexSrc.size();
	outFile.write((const char*)(&indexSize), sizeof(indexSize));
	//Subset ���� ����
	UINT subsetSize = subsetSrc.size();
	outFile.write((const char*)(&subsetSize), sizeof(subsetSize));
	//*******header�� ��������**********


	//���� ����ü ����
	for (int i = 0; i < vertexSize; i++)
	{
		outFile.write((const char*)(&vertexSrc[i]), sizeof(MyVertex::BasicVertex));
	}
	//�ε��� ����
	for (int i = 0; i < indexSize; i++)
	{
		outFile.write((const char*)(&indexSrc[i]), sizeof(UINT));
	}
	//Subset ����
	for (int i = 0; i < subsetSize; i++)
	{
		outFile.write((const char*)(&subsetSrc[i]), sizeof(Subset));
	}

	outFile.close();


	return true;
}