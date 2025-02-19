#pragma once
#include <Stulu.h>
#include "Panel.h"
#include "Previewing.h"

using namespace Stulu;

namespace Editor {
	class MainLayer : public Layer {
	public:
		MainLayer();
		~MainLayer();

		void onAttach() override;
		void onUpdate(Timestep timestep) override;
		void onImguiRender(Timestep timestep) override;
		void onRenderGizmo() override;
		void onEvent(Event& e) override;

		Ref<Scene>& GetActiveScene() { return m_activeScene; }

		//void SaveScene(const std::string& path);
		//void OpenScene(const std::string& path);

		inline bool IsRuntime() { return m_runtime; }
		inline SceneCamera& GetCamera() { return m_sceneCamera; }

		template<class T, class ...Args>
		inline void AddPanel(Args&& ...args) {
			m_panels.insert({ typeid(T).hash_code(), new T(std::forward<Args>(args)...) });
		}
		template<class T>
		inline T& GetPanel() {
			for (auto& [hash, panel] : m_panels) {
				if (hash == typeid(T).hash_code()) {
					return *dynamic_cast<T*>(panel);
				}
			}
			ST_ASSERT(false, "Error panel not found");
			return *reinterpret_cast<T*>(nullptr);
		}
		template<auto Method, class ...Args>
		inline void CallPanels() {
			for (auto& [hash, panel] : m_panels) {
				if (panel) {
					std::invoke(Method, panel, std::forward<Args>(args)...);
				}
			}
		}
	private:
		std::unordered_map<size_t, Panel*> m_panels;

		Ref<FrameBuffer> m_sceneFrameBuffer;

		SceneCamera m_sceneCamera;
		PostProcessingData cameraPostProcessData;
		
		std::string m_currentScenePath;
		GizmoTransformEditMode m_gizmoEditType = GizmoTransformEditMode::None;

		Preview m_preview;

		//void drawObjectOutlines();
		//void onDrawGizmoSelected(GameObject gameObject);
		
		//void drawMenuBar();
		
		//bool onShortCut(KeyDownEvent& e);
		//bool onApplicationQuit(WindowCloseEvent& e);
		
		//void SaveScene();
		//void OpenScene();
		//void NewScene();
		
		//oid OnRuntimeStart();
		//oid OnRuntimeStop();

		Ref<Scene> m_activeScene = nullptr;
		Ref<Scene> m_editorScene = nullptr, m_runtimeScene = nullptr;

		bool m_runtime = false;
	};
}