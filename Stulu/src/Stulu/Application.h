#pragma once

#include "Core.h"
#include "Window.h"
#include "Stulu/LayerStack.h"
#include "Stulu/ImGui/ImGuiLayer.h"
#include "Stulu/Events/ApplicationEvent.h"

#include "Renderer/Shader.h"

namespace Stulu {
	struct GraphicDriverConstants {
		std::string vendor;
		std::string renderer;
		std::string version;
		std::string other;

		GraphicDriverConstants(const std::string& _vendor = "no data", const std::string& _renderer = "no data", const std::string& _version = "no data", const std::string& _other = "no data")
			: vendor(_vendor), renderer(_renderer), version(_version), other(_other) {}
	};
	class STULU_API Application
	{
	public:
		Application();
		virtual ~Application();

		void run();
		void onEvent(Event& e);
		void pushLayer(Layer* layer);
		void pushOverlay(Layer* layer);
		inline Window& getWindow(){ return *m_window; }
		inline static Application& get(){ return *s_instance; }

		GraphicDriverConstants graphicDriverConstants;
	private:
		bool onWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> m_window;
		ImGuiLayer* m_imguiLayer;
		bool m_runnig = true;
		LayerStack m_layerStack;
		
		unsigned int m_vertexArray, m_vertexBuffer, m_indexBuffer;
		std::unique_ptr<Shader> m_shader;


		static Application* s_instance;
	};

	//defined in Client
	Application* CreateApplication();
}

