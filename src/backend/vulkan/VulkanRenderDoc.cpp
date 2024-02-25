#pragma once

#ifdef GRAPHICS_DEBUG
#include "VulkanRenderDoc.h"
#include "Logger.h"
#include <Windows.h>
#if RENDERDOC_ENABLE
#include <renderdoc_app.h>
#endif

namespace MiniEngine
{
namespace Backend
{
namespace VulkanRenderDoc
{
#if defined(RENDERDOC_ENABLE)
	static RENDERDOC_API_1_1_2* rdoc_api = nullptr;
#endif

	void initRenderDoc()
	{
#ifdef RENDERDOC_ENABLE

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
#ifdef RENDERDOC_ENABLE
		if (rdoc_api) rdoc_api->StartFrameCapture(nullptr, nullptr);
#endif
	} 

	void endCapture()
	{
#ifdef RENDERDOC_ENABLE
		if (rdoc_api) rdoc_api->EndFrameCapture(nullptr, nullptr);
#endif
	}

}
}
}

#endif