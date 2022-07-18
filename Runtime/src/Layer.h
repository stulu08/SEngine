#pragma once
#include <Stulu.h>

namespace Stulu {
	class RuntimeLayer : public Layer {
	public:
		RuntimeLayer();
		~RuntimeLayer();

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onEvent(Event& e) override;

		Ref<Scene>& getActiveScene() { return m_activeScene; }

		void OpenScene(const std::string& binpath);
	private:
		struct FbDrawData {
			Ref<FrameBuffer> m_sceneFBo;
			Ref<Shader> m_quadShader;
			Ref<VertexArray> m_quadVertexArray;
		} m_fbDrawData;

		bool onApplicationQuit(WindowCloseEvent& e);
		bool onResize(WindowResizeEvent& e);
		void newScene();
		void onRuntimeStart();
		void onRuntimeStop();

		Ref<Scene> m_activeScene = nullptr;
	};
}