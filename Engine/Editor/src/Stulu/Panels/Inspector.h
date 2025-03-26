#pragma once
#include "Stulu/Panel.h"

#include <Stulu/Scripting/EventCaller.h>

namespace Editor {
	class InspectorRenderer {
	public:
		InspectorRenderer(Stulu::Mono::ReflectionType refType, const std::string& header, Stulu::Mono::Class inspectorClass, int32_t prio, Stulu::ScriptAssembly* assembly)
			: m_refType(refType), m_header(header), m_prio(prio),
			m_object(Stulu::createRef<Stulu::MonoObjectInstance>(inspectorClass, assembly)){}

		InspectorRenderer(InspectorRenderer&&) = default;
		InspectorRenderer& operator=(InspectorRenderer&&) = default;

		InspectorRenderer(const InspectorRenderer&) = delete;
		InspectorRenderer& operator=(const InspectorRenderer&) = delete;

		inline Stulu::Mono::ReflectionType GetType() const {
			return m_refType;
		}
		inline int32_t GetPriority() const {
			return m_prio;
		}
		inline const std::string& GetHeader() const {
			return m_header;
		}
		inline const Stulu::Ref<Stulu::MonoObjectInstance>& GetScriptObject() const {
			return m_object;
		}
	private:
		Stulu::Ref<Stulu::MonoObjectInstance> m_object;
		Stulu::Mono::ReflectionType m_refType;
		std::string m_header;
		int32_t m_prio;
	};

	class InspectorPanel : public Panel {
	public:
		InspectorPanel();
		~InspectorPanel() = default;

		virtual void DrawImGui() override;
		virtual void DrawImGuizmo() override;

		virtual void PreWindow() override;
		virtual void PostWindow() override;
		
		bool HasInspector(Stulu::Mono::ReflectionType refType) const;
	private:
		std::vector<InspectorRenderer> m_inspectors;
		Stulu::Mono::Class m_inspectorClass = nullptr;
		Stulu::Mono::Class m_inspectorAttribute = nullptr;
		Stulu::Mono::Method m_renderMethod = nullptr;
		Stulu::Mono::Method m_renderGizmosMethod = nullptr;
		entt::entity m_nameEditing = entt::null;
		float m_windowIndent = 0.0f;

		void LoadObjects();
		void LoadScriptObjects(Stulu::Ref<Stulu::ScriptAssembly> assembly);
	};
}