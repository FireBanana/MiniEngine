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

		#ifdef GRAPHICS_DEBUG
		if (ImGui::Begin("_graphs"))
		{
			char buffer[32];
			std::sprintf(buffer, "%.2f ms", mFrameTimePoints[POINT_SIZE - 1]);

			ImGui::PlotLines(
				"Full Frame Time",
				mFrameTimePoints,
				POINT_SIZE,
				0,
				buffer,
				1.0f, 2.0f,
				{0, 50});

			ImGui::End();
		}
		#endif

		// Render Sliders
		for (auto& [name, v, min, max, cb] : mSliderPanels)
		{
			if (ImGui::Begin("Sliders"))
			{
				if (ImGui::SliderFloat(name, v, min, max) && cb != nullptr) cb();
				ImGui::End();
			}
		}

		for (auto& [name, flag, cb] : mCheckboxPanels)
		{
			if (ImGui::Begin("Checkboxes"))
			{
				if (ImGui::Checkbox(name, &flag) && cb != nullptr) cb();
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

	void OpenGLImgui::createBooleanPanel(const char* name, bool& flag, std::function<void()> cb)
	{
		mCheckboxPanels.push_back({ name, flag, cb });
	}
	
	void OpenGLImgui::pushFrameTimeData(float deltaTime)
	{
		static int pointer = 0;

		if (pointer < POINT_SIZE)
			mFrameTimePoints[pointer++] = deltaTime;
		else
		{
			for (int i = 0; i < POINT_SIZE - 1; ++i)
			{
				mFrameTimePoints[i] = mFrameTimePoints[i + 1];
			}

			mFrameTimePoints[POINT_SIZE - 1] = deltaTime;
		}
	}
}
