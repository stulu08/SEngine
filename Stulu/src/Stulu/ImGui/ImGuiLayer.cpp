#include "st_pch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "imgui_internal.h"
#include "ImGuizmo.h"
#include "Gizmo.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Core/Input.h"
#ifdef USING_GLFW
	#include "backends/imgui_impl_glfw.h"
	#include <GLFW/glfw3.h>
#endif
#ifdef OPENGL
	#include "backends/imgui_impl_opengl4.h"
#endif


namespace Stulu {


	ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer"){}

	ImGuiLayer::~ImGuiLayer(){
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

#ifdef USING_GLFW
		ImGui_ImplGlfw_InitForOpenGL(window, true);
	#ifdef OPENGL
			ImGui_ImplOpenGL4_Init("#version 460");
	#endif
#endif

		Gizmo::init();
	}

	void ImGuiLayer::onDetach() {
		ST_PROFILING_FUNCTION();
#ifdef USING_GLFW
	#ifdef OPENGL
			ImGui_ImplOpenGL4_Shutdown();
	#endif
		ImGui_ImplGlfw_Shutdown();
#endif
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin() {
		ST_PROFILING_FUNCTION();
#ifdef USING_GLFW
	#ifdef OPENGL
			ImGui_ImplOpenGL4_NewFrame();
	#endif
		ImGui_ImplGlfw_NewFrame();
#endif


		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}
	void ImGuiLayer::End() {
		ST_PROFILING_FUNCTION();
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2((float)app.getWindow().getWidth(), (float)app.getWindow().getHeight());
		// Rendering
		ImGui::Render();
#ifdef USING_GLFW
	#ifdef OPENGL
			ImGui_ImplOpenGL4_RenderDrawData(ImGui::GetDrawData());
	#endif
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}
#endif
	}
	void ImGuiLayer::onImguiRender(Timestep timestep) {
		
	}

	void ImGuiLayer::onEvent(Event& e) {
		ST_PROFILING_FUNCTION();
		if (m_blockEvents) {
			ImGuiIO& io = ImGui::GetIO();
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

void ImGui::Image(const Stulu::Ref<Stulu::Texture>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col) {
	ImGui::Image(StuluTextureToImGui(texture), size, uv0, uv1, tint_col, border_col);
}

void ImGui::Image(const Stulu::Ref<Stulu::Texture>& texture, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& tint_col, const glm::vec4& border_col) {
	ImGui::Image(StuluTextureToImGui(texture), { size.x,  size.y }, { uv0.x,  uv0.y }, { uv1.x,  uv1.y }, { tint_col.x, tint_col.y, tint_col.z, tint_col.w }, { border_col.x, border_col.y, border_col.z, border_col.w });
}

bool ImGui::ImageButton(const Stulu::Ref<Stulu::Texture>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col) {
	return ImGui::ImageButton(StuluTextureToImGui(texture), size, uv0, uv1,frame_padding, bg_col, tint_col);
}

bool ImGui::ImageButton(const Stulu::Ref<Stulu::Texture>& texture, const glm::vec2& size, const glm::vec2& uv0, const glm::vec2& uv1, int frame_padding, const glm::vec4& bg_col, const glm::vec4& tint_col) {
	return ImGui::ImageButton(StuluTextureToImGui(texture), {size.x,  size.y}, {uv0.x,  uv0.y}, {uv1.x,  uv1.y}, frame_padding, {bg_col.x, bg_col.y, bg_col.z, bg_col.w}, {tint_col.x, tint_col.y, tint_col.z, tint_col.w});
}

ImTextureID ImGui::StuluTextureToImGui(const Stulu::Ref<Stulu::Texture>& texture) {
	ImTextureID id = nullptr;
	if (Stulu::Renderer::getRendererAPI() == Stulu::RenderAPI::API::OpenGL || Stulu::Renderer::getRendererAPI() == Stulu::RenderAPI::API::GLES)
		id = reinterpret_cast<void*>((uint64_t)(*(uint32_t*)texture->getNativeRendererObject()));

	return id;
}
