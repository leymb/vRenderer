#pragma once

#include <fstream>
#include <string>
#include <vector>

static std::vector<char> ReadFile(const std::string& a_FilePath)
{
	std::ifstream t_File(a_FilePath, std::ios::ate | std::ios::binary);

	if (!t_File.is_open())
	{
		throw std::runtime_error("Failed to open file!");
	}

	// use read position to determine file size
	const size_t t_FileSize = t_File.tellg();

	// allocate buffer for the data
	std::vector<char> t_Buffer(t_FileSize);

	// read the contents of the file
	t_File.seekg(0);
	t_File.read(t_Buffer.data(), t_FileSize);

	// close file
	t_File.close();

	return t_Buffer;
}
