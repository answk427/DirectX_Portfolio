#include "FileReaderWriter.h"

std::vector<FileReaderWriter*> FileReaderWriter::FRWs;


FileReaderWriter::FileReaderWriter(const fileTypeStr & type, const std::wstring & directory)
{
	fileType = type;
	fileDirectory = std::wstring(fs::current_path().c_str()) + L"/" + directory;
}

FileReaderWriter::FileReaderWriter(const fileTypeStr & type, const std::string & directory)
{
	fileType = type;
	fileDirectory = std::wstring(fs::current_path().c_str()) + +L"/" + ConvertWSTR(directory);
}

bool FileReaderWriter::FileExist(const FR_fileName & fileName, int mode)
{
	//파일이 존재하는지 확인
	if (_waccess(fileName.c_str(), 0) != -1)
		return true;
	else
		return false;
}

bool FileReaderWriter::FileExist(const std::string & fileName, int mode)
{
	//파일이 존재하는지 확인
	if (access(fileName.c_str(), 0) != -1)
		return true;
	else
		return false;
}

FR_fileName FileReaderWriter::GetFileFullName(const FR_fileName & fileName)
{
	FR_fileName fullFileName = fileDirectory + fileName + ConvertWSTR(fileType);
	return fullFileName;
}
