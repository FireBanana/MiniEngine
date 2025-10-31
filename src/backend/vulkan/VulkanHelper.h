#pragma once

#ifndef VK_HELPER_INCLUDED
#define VK_HELPER_INCLUDED
#include "Logger.h"
#include <volk.h>

#ifdef GRAPHICS_DEBUG
inline void create_debug_name(
    VkDevice activeDevice, std::string name, VkObjectType objectType, uint64_t objectHandle)
{
    VkDebugUtilsObjectNameInfoEXT debugInfo{};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
    debugInfo.pNext = NULL;
    debugInfo.objectType = objectType;
    debugInfo.objectHandle = objectHandle;
    debugInfo.pObjectName = name.c_str();

    if (vkSetDebugUtilsObjectNameEXT(activeDevice, &debugInfo) != VK_SUCCESS)
        MiniEngine::Logger::eprint("Debug name creationg failed for: {}", name);
}

#endif
#endif
