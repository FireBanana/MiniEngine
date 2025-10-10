#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#include "Logger.h"

namespace FileHelper {
static std::string loadFile(const char *path)
{
    try {
        auto stream = std::fstream(path, std::fstream::in);
        std::stringstream buffer;
        buffer << stream.rdbuf();

        return buffer.str();
    } catch (...) {
        MiniEngine::Logger::eprint("Error occured while reading file");
        return "";
    }
}

}; // namespace FileHelper
