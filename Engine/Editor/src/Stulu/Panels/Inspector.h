#pragma once
#include "Stulu/Panel.h"

#include <Stulu/Scripting/EventCaller.h>

namespace Editor {
	class InspectorRenderer {
	public:
		InspectorRenderer(Stulu::Mono::ReflectionType refType, const std::string& header, Stulu::Mono::Class inspectorClass, Stulu::ScriptAssembly* assembly)
			: m_refType(refType), m_header(header), 
			m_object(Stulu::createRef<Stulu::MonoObjectInstance>(inspectorClass, assembly)){}

		InspectorRenderer(InspectorRenderer&&) = default;

		inline Stulu::Mono::ReflectionType GetType() const {
			return m_refType;
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
	};

	class InspectorPanel : public Panel {
	public:
		InspectorPanel();
		~InspectorPanel() = default;

		virtual void DrawImGui() override;
		
	private:
		std::vector<InspectorRenderer> m_inspectors;
		Stulu::Mono::Class m_inspectorClass = nullptr;
		Stulu::Mono::Class m_inspectorAttribute = nullptr;
		Stulu::Mono::Method m_renderMethod = nullptr;

		void LoadObjects();
		void LoadScriptObjects(Stulu::Ref<Stulu::ScriptAssembly> assembly);
	};
}