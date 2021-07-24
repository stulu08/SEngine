#include "st_pch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "Platform/OpenGL/ImGuiOpengGLRenderer.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <Stulu/Application.h>

namespace Stulu {
	Stulu::ImGuiLayer::ImGuiLayer()
		:Layer("ImGuiLayer")
	{

	}

	Stulu::ImGuiLayer::~ImGuiLayer()
	{

	}
	void Stulu::ImGuiLayer::onUpdate()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::get();
		io.DisplaySize = ImVec2(app.getWindow().getWidth(), app.getWindow().getHeight());

		float time = (float)glfwGetTime();
		io.DeltaTime = m_time > 0.0f ? (time - m_time) : (1.0f / 60.0f);
		m_time = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();


		static bool show = true;
		ImGui::ShowDemoWindow(&show);


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void Stulu::ImGuiLayer::onEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.dispacth<MouseButtonDownEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onMouseButtonDownEvent));
		dispatcher.dispacth<MouseButtonUpEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onMouseButtonUpEvent));
		dispatcher.dispacth<MouseScrollEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onMouseScrollEvent));
		dispatcher.dispacth<MouseMoveEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onMouseMoveEvent));

		dispatcher.dispacth<KeyTypedEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));
		dispatcher.dispacth<KeyDownEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onKeyDownEvent));
		dispatcher.dispacth<KeyUpEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onKeyUpEvent));

		dispatcher.dispacth<WindowResizeEvent>(ST_BIND_EVENT_FN(ImGuiLayer::onWindowResizeEvent));
	}
	static const char* ImGui_GetClipboardText(void* user_data);
	static void ImGui_SetClipboardText(void* user_data, const char* text);
	void ImGuiLayer::onAttach()
	{
		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//temp
		io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
		io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		io.SetClipboardTextFn = ImGui_SetClipboardText;
		io.GetClipboardTextFn = ImGui_GetClipboardText;
		//io.ClipboardUserData = g_Window;

		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::onDetach()
	{

	}
	static const char* ImGui_GetClipboardText(void* user_data)
	{
		return glfwGetClipboardString((GLFWwindow*)user_data);
	}

	static void ImGui_SetClipboardText(void* user_data, const char* text)
	{
		glfwSetClipboardString((GLFWwindow*)user_data, text);
	}
	bool ImGuiLayer::onMouseButtonDownEvent(MouseButtonDownEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getButton()] = true;

		return false;
	}

	bool ImGuiLayer::onMouseButtonUpEvent(MouseButtonUpEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseDown[e.getButton()] = false;

		return false;
	}

	bool ImGuiLayer::onMouseMoveEvent(MouseMoveEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MousePos = ImVec2(e.getX(), e.getY());

		return false;
	}

	bool ImGuiLayer::onMouseScrollEvent(MouseScrollEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.MouseWheelH += e.getXOff();
		io.MouseWheel += e.getYOff();

		return false;
	}

	bool ImGuiLayer::onKeyDownEvent(KeyDownEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = true;

		io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		return false;
	}
	bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e) {
		ImGuiIO& io = ImGui::GetIO();
		int keycode = e.getKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter((unsigned short)keycode);

		return false;
	}
	bool ImGuiLayer::onKeyUpEvent(KeyUpEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[e.getKeyCode()] = false;

		

		return false;
	}

	bool ImGuiLayer::onWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, e.getWidth(), e.getHeight());
		return false;
	}

}