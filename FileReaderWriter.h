#pragma once

#include "ReaderWriterConstant.h"
#include "UtilFunctions.h"
#include <fstream>
#include <io.h>
#include <filesystem>


namespace fs = std::filesystem;
typedef std::wstring FR_fileName;
typedef std::string fileTypeStr;


class FileReaderWriter
{
protected:
	fileTypeStr fileType;
	std::wstring fileDirectory;
public:
	static std::vector<FileReaderWriter*> FRWs;
public:
	FileReaderWriter(const fileTypeStr& type, const std::wstring& directory);
	FileReaderWriter(const fileTypeStr& type, const std::string& directory);
	
	virtual ~FileReaderWriter() {}

public:
	virtual bool ReadFile(const FR_fileName& fileName, char* dest) = 0;
	virtual bool WriteFile(void* data) = 0;

public:
	virtual void AddFRW(FileReaderWriter* frw) { FRWs.push_back(frw); }
	const fileTypeStr& getFileType() { return fileType; }
	
	bool FileExist(const FR_fileName& fileName, int mode);
	bool FileExist(const std::string& fileName, int mode);

	//directory 경로까지 포함한 파일 위치 반환
	FR_fileName GetFileFullName(const FR_fileName& fileName);
	

	
};


