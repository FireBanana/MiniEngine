#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <vector>

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

		void createSliderPanel(float* value, float min, float max);

	private:

		std::vector<std::tuple< float*, float, float >> mSliderPanels;

	};
}
}