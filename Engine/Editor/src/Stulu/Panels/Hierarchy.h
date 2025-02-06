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

		inline void SetScene(Stulu::Ref<Stulu::Scene> scene) {
			m_scene = scene.get();
		}
		inline void AddSelected(entt::entity gameObject) {
			m_selected.push_back(gameObject);
		}
		inline void SetSelected(entt::entity gameObject) {
			m_selected.clear();
			m_selected.push_back(gameObject);
		}
		inline bool IsSelected(entt::entity gameObject) const {
			return std::find(m_selected.begin(), m_selected.end(), gameObject) != m_selected.end();
		}
	private:
		std::vector<entt::entity> m_selected;
		std::vector<Stulu::GameObject> m_childObjectsBuffer;
		Stulu::Scene* m_scene;

		inline entt::registry& GetRegistry() {
			return m_scene->getRegistry();
		}

		void DrawGameObject(const Stulu::GameObjectBaseComponent& baseComponent, const Stulu::GameObject& parent = Stulu::GameObject::null);

		void DummyDragDropTarget(const Stulu::GameObject& parent);
		void DragDropTarget(const Stulu::GameObject& parent);
		void DragDropSource();

		void LoadChilds(const Stulu::GameObject& parent);
		void SetParents(const Stulu::GameObject& parent, entt::entity* targets, size_t count);
	};
}