#pragma once
#include "FileReaderWriter.h"
#include "Mesh.h"
#include <direct.h>



class MeshReaderWriter : public FileReaderWriter
{
public:
	MeshReaderWriter() : FileReaderWriter(FILETYPE_MESH, FOLDERNAME_MESH) { AddFRW(this); }
		
	// FileReaderWriter을(를) 통해 상속됨
	virtual bool WriteFile(void * data) override;

	// FileReaderWriter을(를) 통해 상속됨
	virtual bool ReadFile(const FR_fileName& fileName, char* dest) override;

	// FileReaderWriter을(를) 통해 상속됨
	static MeshReaderWriter& Instance();

};
