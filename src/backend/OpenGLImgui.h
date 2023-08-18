#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <functional>
#include <vector>
#include "ThreadSafeQueue.h"

namespace MiniEngine
{
namespace Backend 
{

	class OpenGLImgui
	{
	public:

		OpenGLImgui();

		void Initialize(GLFWwindow* window);
		void draw();

		void createSliderPanel(const char* name, float* value, float min, float max, std::function<void()> cb);

		void pushFrameTimeData(float deltaTime);

	private:

		std::vector< std::tuple< 
			const char*,
			float*,
			float,
			float,
			std::function<void()> 
			> > mSliderPanels;

		#ifdef GRAPHICS_DEBUG
		Utils::ThreadSafeQueue<float, 32> mFrameTimeQueue;
		#endif
	};
}
}