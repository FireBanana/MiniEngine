#include "OpenGLImgui.h"

namespace MiniEngine::Backend
{
	OpenGLImgui::OpenGLImgui()
	{
	}

	void OpenGLImgui::Initialize(GLFWwindow* window)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		ImGui::StyleColorsDark();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 450");
	}

	void OpenGLImgui::draw()
	{
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		auto& io = ImGui::GetIO();

		// Render Sliders
		for (auto& [name, v, min, max, cb] : mSliderPanels)
		{
			if (ImGui::Begin("Slider"))
			{
				if (ImGui::SliderFloat(name, v, min, max) && cb != nullptr) cb();
				ImGui::End();
			}
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}
	void OpenGLImgui::createSliderPanel(const char* name, float* value, float min, float max, std::function<void()> cb)
	{
		mSliderPanels.push_back({name, value, min, max, cb});
	}
}
