#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "IImgui.h"

#include <functional>
#include <vector>
#include "ThreadSafeQueue.h"

namespace MiniEngine
{
namespace Backend 
{
	constexpr int POINT_SIZE = 16;

	class OpenGLImgui : public IImgui
	{
	public:

		OpenGLImgui();

		void initialize(GLFWwindow* window);
		void draw();

		void createSliderPanel(const char* name, float* value, float min, float max, std::function<void()> cb) override;
		void createBooleanPanel(const char* name, bool& flag, std::function<void()> cb) override;

		void pushFrameTimeData(float deltaTime);

	private:

		std::vector< std::tuple< 
			const char*,
			float*,
			float,
			float,
			std::function<void()> 
			> > mSliderPanels;

		std::vector< std::tuple<
			const char*,
			bool& ,
			std::function<void()>
			> > mCheckboxPanels;

		#ifdef GRAPHICS_DEBUG
		float mFrameTimePoints[POINT_SIZE];
		#endif
	};
}
}