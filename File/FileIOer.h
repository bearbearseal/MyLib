#pragma once
#include <string>

class FileIOer
{
public:
	FileIOer() {}
	FileIOer(const char* const fileName);
	FileIOer(const std::string fileName);
	~FileIOer();
	void open(const char* const fileName);
	size_t append(const void* const data, uint32_t length);
	bool read_data(uint32_t position, void* const dest, uint32_t length);
	bool read_data(uint32_t startIndex, std::string& dest, uint32_t length);
	bool read_data(std::string& dest, uint32_t startIndex = 0);
	std::string read_data(uint32_t startIndex = 0);
	bool change_data(uint32_t position, const void* const newData, uint32_t length);
	bool write_new(const std::string& newData);
	bool write_new(const char* const newData);
	std::string get_name() { return fileName; }
	size_t size();
	bool clear();

	static bool file_exist(const std::string& name);
	static bool create_folder(const std::string& name);

private:
	std::string fileName;
};
