#pragma once
#include "Stulu/Core/Layer.h"
#include "Stulu/Scene/Scene.h"

namespace Stulu {
	class STULU_API RuntimeLayer : public Layer {
	public:
		RuntimeLayer(const std::string& startScene);
		virtual ~RuntimeLayer();

		virtual void onAttach() override;
		virtual void onUpdate(Timestep timestep) override;
		virtual void onEvent(Event& e) override;

		Ref<Scene>& getActiveScene() { return m_activeScene; }
		void OpenScene(const std::string& binpath);
	protected:
		virtual bool onApplicationQuit(WindowCloseEvent& e);
		virtual bool onResize(WindowResizeEvent& e);
		void newScene();
		virtual void onRuntimeStart();
		virtual void onRuntimeStop();
	private:
		Ref<Scene> m_activeScene = nullptr;
		Ref<FrameBuffer> m_frameBuffer = nullptr;
	};
}