#pragma once
#include "Stulu/Panel.h"

#include <Stulu/Scene/Scene.h>
#include <Stulu/Scene/GameObject.h>
#include <Stulu/Scene/Components/Components.h>

namespace Editor {
	class HierarchyPanel : public Panel {
	public:
		HierarchyPanel(Stulu::Ref<Stulu::Scene> scene = nullptr);
		virtual ~HierarchyPanel() = default;

		virtual void DrawImGui() override;
		virtual void DrawImGuizmo() override;

		virtual void PreWindow() override;
		virtual void PostWindow() override;

		inline void SetScene(Stulu::Ref<Stulu::Scene> scene) {
			m_scene = scene.get();
			m_selected.clear();
		}

		inline void SelectedLogicAdd(entt::entity gameObject) {
			Stulu::GameObject go = { gameObject, m_scene };

			if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl)) {
				AddSelected(gameObject);
			}
			else if (IsSelected(gameObject) && ImGui::IsKeyDown(ImGuiKey_LeftShift) && go.getComponent<Stulu::TransformComponent>().HasParent()) {
				AddSelected(go.getComponent<Stulu::TransformComponent>().GetParent().GetID());
				RemoveSelected(gameObject);
			}
			else {
				SetSelected(gameObject);
			}
		}
		inline void AddSelected(entt::entity gameObject) {
			if (IsSelected(gameObject)) {
				RemoveSelected(gameObject);
			}
			else {
				m_selected.push_back(gameObject);
			}
		}
		inline void RemoveSelected(entt::entity gameObject) {
			m_selected.erase(std::remove(m_selected.begin(), m_selected.end(), gameObject), m_selected.end());
		}
		inline void SetSelected(entt::entity gameObject) {
			m_selected.clear();
			m_selected.push_back(gameObject);
		}
		inline bool IsSelected(entt::entity gameObject) const {
			return std::find(m_selected.begin(), m_selected.end(), gameObject) != m_selected.end();
		}
		inline const std::vector<entt::entity>& GetSelected() const {
			return m_selected;
		}
	private:
		std::vector<entt::entity> m_selected;
		Stulu::Scene* m_scene;
		std::string m_search;
		float m_windowPaddingOriginal = 0.0f;

		inline entt::registry& GetRegistry() {
			return m_scene->GetRegistry();
		}

		void DrawGameObject(Stulu::GameObjectBaseComponent& baseComponent, const Stulu::GameObject& parent = Stulu::GameObject::null);

		void DummyDragDropTarget(const Stulu::GameObject& parent);
		void DragDropTarget(const Stulu::GameObject& parent);
		void DragDropSource();

		void SetParents(const Stulu::GameObject& parent, const std::vector<entt::entity> children);
	};
}