#pragma once

#ifndef VK_HELPER_INCLUDED
#define VK_HELPER_INCLUDED

#include <volk.h>

#include "Logger.h"

namespace MiniEngine
{
namespace Backend
{

#define VK_CHECK(x)														  \
	do																	  \
	{																	  \
		VkResult err = x;												  \
		if (err)														  \
		{																  \
			MiniEngine::Logger::eprint("Detected Vulkan error: {}", err); \
		}																  \
	} while (0)

}
}

#endif