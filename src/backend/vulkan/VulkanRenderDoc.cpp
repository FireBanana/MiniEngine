#pragma once

#ifdef GRAPHICS_DEBUG
#include "VulkanRenderDoc.h"
#include "Logger.h"
#include <renderdoc_app.h>
#include <Windows.h>

namespace MiniEngine
{
namespace Backend
{
namespace VulkanRenderDoc
{
	static RENDERDOC_API_1_1_2* rdoc_api = nullptr;

	void initRenderDoc()
	{
#ifdef GRAPHICS_DEBUG

		// Windows only

		if (HMODULE mod = GetModuleHandleA("renderdoc.dll"))
		{
			pRENDERDOC_GetAPI RENDERDOC_GetAPI =
				(pRENDERDOC_GetAPI)GetProcAddress(mod, "RENDERDOC_GetAPI");
			int ret = RENDERDOC_GetAPI(eRENDERDOC_API_Version_1_1_2, (void**)&rdoc_api);

			if (ret != 1) MiniEngine::Logger::eprint("RenderDoc not found!");
		}
#endif
	}

	void startCapture()
	{
#ifdef GRAPHICS_DEBUG
		if (rdoc_api) rdoc_api->StartFrameCapture(nullptr, nullptr);
#endif
	} 

	void endCapture()
	{
#ifdef GRAPHICS_DEBUG
		if (rdoc_api) rdoc_api->EndFrameCapture(nullptr, nullptr);
#endif
	}

}
}
}

#endif