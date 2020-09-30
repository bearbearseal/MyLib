#include "FileIOer.h"
#include <cstring>

FileIOer::FileIOer(const std::string _fileName)
{
	fileName = _fileName;
}

FileIOer::FileIOer(const char* const _fileName)
{
	fileName.assign(_fileName);
}

FileIOer::~FileIOer()
{

}

void FileIOer::open(const char* const _fileName)
{
	fileName.assign(_fileName);
}

size_t FileIOer::append(const void* const data, uint32_t length)
{
	FILE* file;
	signed result;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "a+b");
#else
	file = fopen(fileName.c_str(), "a+b");
#endif

	if (file == NULL)
	{
		return 0;
	}
	fwrite(data, length, 1, file);
	fseek(file, 0, SEEK_END);
	result = ftell(file);
	fclose(file);
	return result;
}

bool FileIOer::write_new(const std::string& newData) {
	return write_new(newData.c_str());
}

bool FileIOer::write_new(const char* const newData) {
	FILE* file;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "w+b");
#else
	file = fopen(fileName.c_str(), "w+b");
#endif
	if (file == NULL) {
		return false;
	}
	fwrite(newData, strlen(newData), 1, file);
	fclose(file);
	return true;
}

bool FileIOer::read_data(uint32_t startIndex, std::string& dest, uint32_t length) {
	FILE* file;
	size_t readLength;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "r+b");
#else
	file = fopen(fileName.c_str(), "r+b");
#endif
	if (file == NULL)
	{
		return false;
	}
	char* buffer = new char[length];
	fseek(file, startIndex, SEEK_SET);
#ifdef _WIN32
	readLength = fread_s(buffer, length, 1, length, file);
#else
	readLength = fread(buffer, 1, length, file);
#endif
	dest.assign(buffer, readLength);
	delete[] buffer;
	fclose(file);
	return true;
}

bool FileIOer::read_data(std::string& dest, uint32_t startIndex) {
	FILE* file;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "r+b");
#else
	file = fopen(fileName.c_str(), "r+b");
#endif
	if (file == NULL)
	{
		//printf("File not found.\n");
		return false;
	}
	fseek(file, 0, SEEK_END);
	size_t readLength = ftell(file) - startIndex;
	char* buffer = new char[readLength];
	fseek(file, startIndex, SEEK_SET);
#ifdef _WIN32
	readLength = fread_s(buffer, readLength, 1, readLength, file);
#else
	readLength = fread(buffer, 1, readLength, file);
#endif
	dest.assign(buffer, readLength);
	delete[] buffer;
	fclose(file);
	return true;
}


bool FileIOer::read_data(uint32_t position, void* const dest, uint32_t length)
{
	FILE* file;
	//size_t readLength;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "r+b");
#else
	file = fopen(fileName.c_str(), "r+b");
#endif
	if (file == NULL)
	{
		return false;
	}
	fseek(file, position, SEEK_SET);
#ifdef _WIN32
	fread_s(dest, length, 1, length, file);
#else
	fread(dest, 1, length, file);
#endif
	fclose(file);
	return true;
}

std::string FileIOer::read_data(uint32_t startIndex) {
	FILE* file;
	std::string retVal;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "r+b");
#else
	file = fopen(fileName.c_str(), "r+b");
#endif
	if (file != NULL)
	{
		fseek(file, 0, SEEK_END);
		size_t readLength = ftell(file) - startIndex;
		char* buffer = new char[readLength];
		fseek(file, startIndex, SEEK_SET);
#ifdef _WIN32
		readLength = fread_s(buffer, readLength, 1, readLength, file);
#else
		readLength = fread(buffer, 1, readLength, file);
#endif
		retVal.assign(buffer, readLength);
		delete[] buffer;
		fclose(file);
	}
	return retVal;
}


bool FileIOer::change_data(uint32_t position, const void* const newData, uint32_t length)
{
	FILE* file;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "r+b");
#else
	file = fopen(fileName.c_str(), "r+b");
#endif
	if (file == NULL)
	{
		return false;
	}
	fseek(file, position, SEEK_SET);
	fwrite(newData, length, 1, file);
	fclose(file);
	return true;
}

size_t FileIOer::size()
{
	FILE* file;
	size_t result;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "rb");
#else
	file = fopen(fileName.c_str(), "rb");
#endif
	if (file == NULL)
	{
		return 0;
	}
	fseek(file, 0, SEEK_END);
	result = ftell(file);
	fclose(file);
	return result;
}

bool FileIOer::clear()
{
	FILE* file;
#ifdef _WIN32
	fopen_s(&file, fileName.c_str(), "w+b");
#else
	file = fopen(fileName.c_str(), "w+b");
#endif
	if (file == NULL)
	{
		return false;
	}
	fclose(file);
	return true;
}
