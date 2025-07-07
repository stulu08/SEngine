#pragma once
#include <Stulu.h>

#include "Panel.h"
#include "Previewing.h"
#include "Shortcut.h"

#include "Panels/Scene.h"
#include "Panels/Game.h"
#include "Panels/LogPanel.h"

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

		void onLoadFinish();

		inline bool IsRuntime() const { return m_runtime; }
		inline Ref<Scene>& GetActiveScene() { return IsRuntime() ? m_runtimeScene : m_editorScene; }
		inline const Ref<Scene>& GetActiveScene() const { return IsRuntime() ? m_runtimeScene : m_editorScene; }

		void SaveScene(const std::string& path = "", bool browse = false);
		void OpenScene(const std::string& path);
		void OpenScene();

		void StartRuntime();
		void StopRuntime();

		void AddShortCut(Shortcut&& shortcut) {
			m_shortcuts.push_back(std::move(shortcut));
		}
		std::vector<Shortcut>& GetShortcuts() {
			return m_shortcuts;
		}

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
		inline void CallPanels(Args&& ... args) {
			for (auto& [hash, panel] : m_panels) {
				if (panel) {
					std::invoke(Method, panel, std::forward<Args>(args)...);
				}
			}
		}
	private:
		std::unordered_map<size_t, Panel*> m_panels;

		std::string m_currentScenePath;

		Preview m_preview;
		ScenePanel* m_scenePanel;
		GamePanel* m_gamePanel;
		LogPanel* m_logPanel;
		std::vector<Shortcut> m_shortcuts;

		void DrawObjectOutlines();
		void DrawObjectToStencil(entt::entity obj);
		void DrawObjectStencilOutline(entt::entity obj);


		void DrawGizmoSelected(GameObject gameObject);

		void DrawMenuBar();
		
		//bool onShortCut(KeyDownEvent& e);
		//bool onApplicationQuit(WindowCloseEvent& e);
		
		void LoadShotcuts();
		bool CheckShortcuts();

		void OpenScene(Ref<Scene> scene);
		inline void NewScene() {
			OpenScene(createRef<Scene>());
		}

		void SetScene(Ref<Scene> scene);

		Ref<Scene> m_editorScene = nullptr, m_runtimeScene = nullptr;

		bool m_runtime = false;
	};
}