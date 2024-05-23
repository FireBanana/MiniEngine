#pragma once

#include <fstream>
#include <sstream>
#include <iostream>

#include "Logger.h"

#define RELATIVE_PATH(path) DIR##path

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
			MiniEngine::Logger::eprint("Error occured while reading file");
			return "";
		}
	}

};
