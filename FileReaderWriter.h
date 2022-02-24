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
	FileReaderWriter(const fileTypeStr& type, const std::wstring& directory) 
	{
		fileType = type; 
		fileDirectory = std::wstring(fs::current_path().c_str()) + L"/" + directory;
	}
	FileReaderWriter(const fileTypeStr& type, const std::string& directory)
	{
		fileType = type;
		fileDirectory = std::wstring(fs::current_path().c_str()) + +L"/"+ConvertWSTR(directory);
	}
	virtual ~FileReaderWriter() {}

public:
	virtual bool ReadFile(const FR_fileName& fileName, char* dest) = 0;
	virtual bool WriteFile(void* data) = 0;
	virtual void AddFRW(FileReaderWriter* frw) { FRWs.push_back(frw); }
	
	
	bool FileExist(const FR_fileName& fileName, int mode)
	{
		//파일이 존재하는지 확인
		if (_waccess(fileName.c_str(), 0) != -1)
			return true;
		else
			return false;
	}
	bool FileExist(const std::string& fileName, int mode)
	{
		//파일이 존재하는지 확인
		if (access(fileName.c_str(), 0) != -1)
			return true;
		else
			return false;
	}

	const fileTypeStr& getFileType() { return fileType; }

};
