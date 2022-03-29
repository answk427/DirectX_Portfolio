#pragma once
#include "FileReaderWriter.h"
#include "Mesh.h"
#include <direct.h>



class MeshReaderWriter : public FileReaderWriter
{
public:
	MeshReaderWriter() : FileReaderWriter(FILETYPE_MESH, FOLDERNAME_MESH) { AddFRW(this); }
		
	// FileReaderWriter��(��) ���� ��ӵ�
	virtual bool WriteFile(void * data) override;

	// FileReaderWriter��(��) ���� ��ӵ�
	virtual bool ReadFile(const FR_fileName& fileName, char* dest) override;

	// FileReaderWriter��(��) ���� ��ӵ�
	static MeshReaderWriter& Instance();

};
