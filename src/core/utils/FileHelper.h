#pragma once

#include <fstream>
#include <sstream>
#include <iostream>

class FileHelper
{
public:

	static std::string loadFile(const char* path)
	{
		try
		{
			auto stream = std::fstream(path, std::fstream::in);
			std::stringstream buffer;
			buffer << stream.rdbuf();

			return buffer.str();
		}
		catch (...)
		{
			std::cout << "Error occured while reading file";
			return "";
		}
	}

};