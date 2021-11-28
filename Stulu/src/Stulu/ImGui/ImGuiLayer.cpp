#include "st_pch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

#include "ImGuizmo.h"
#include "Stulu/Core/Application.h"

#include <GLFW/glfw3.h>


namespace Stulu {


	ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer"){}

	ImGuiLayer::~ImGuiLayer(){
		ST_PROFILING_FUNCTION();
	}

	void ImGuiLayer::onAttach() {
		ST_PROFILING_FUNCTION();
		IMGUI_CHECKVERSION();
		m_context = ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}


		Application& app = Application::get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 460");
	}

	void ImGuiLayer::onDetach()
	{
		ST_PROFILING_FUNCTION();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ST_PROFILING_FUNCTION();
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}
	void ImGuiLayer::End()
	{
		ST_PROFILING_FUNCTION();
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2((float)app.getWindow().getWidth(), (float)app.getWindow().getHeight());
		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
	}
	void ImGuiLayer::onImguiRender(Timestep timestep) {
	}

	void ImGuiLayer::onEvent(Event& e) {
		if (m_blockEvents) {
			ImGuiIO& io = ImGui::GetIO();
			e.handled |= e.isInCategory(MouseEventCategrory) & io.WantCaptureKeyboard;
			e.handled |= e.isInCategory(KeyboardEventCategrory) & io.WantCaptureKeyboard;
		}
	}


}

ImVec2 operator*(const ImVec2& vec, const float v) {
	return ImVec2(vec.x * v, vec.y * v);
}
ImVec2 operator/(const ImVec2& vec, const float v) {
	return ImVec2(vec.x / v, vec.y / v);
}
ImVec2 operator+(const ImVec2& vec, const float v) {
	return ImVec2(vec.x + v, vec.y + v);
}
ImVec2 operator-(const ImVec2& vec, const float v) {
	return ImVec2(vec.x - v, vec.y - v);
}
ImVec2 operator*(const ImVec2& vec, const ImVec2& vec2) {
	return ImVec2(vec.x * vec2.x, vec.y * vec2.y);
}
ImVec2 operator/(const ImVec2& vec, const ImVec2& vec2) {
	return ImVec2(vec.x / vec2.x, vec.y / vec2.y);
}
ImVec2 operator+(const ImVec2& vec, const ImVec2& vec2) {
	return ImVec2(vec.x + vec2.x, vec.y + vec2.y);
}
ImVec2 operator-(const ImVec2& vec, const ImVec2& vec2) {
	return ImVec2(vec.x - vec2.x, vec.y - vec2.y);
}
ImVec2 operator*(const ImVec2& vec, const glm::vec2& vec2) {
	return ImVec2(vec.x * vec2.x, vec.y * vec2.y);
}
ImVec2 operator/(const ImVec2& vec, const glm::vec2& vec2) {
	return ImVec2(vec.x / vec2.x, vec.y / vec2.y);
}
ImVec2 operator+(const ImVec2& vec, const glm::vec2& vec2) {
	return ImVec2(vec.x + vec2.x, vec.y + vec2.y);
}
ImVec2 operator-(const ImVec2& vec, const glm::vec2& vec2) {
	return ImVec2(vec.x - vec2.x, vec.y - vec2.y);
}
