#pragma once
#include "Stulu/Panel.h"

#include <Stulu/Scripting/Managed/Bindings/Bindings.h>
#include <Stulu/Scripting/Managed/Bindings/Components/GameObject.h>

namespace Editor {
	bool TreeNodeInspector(const std::string& name, bool* outBtn = NULL, const std::string& btnText = NULL);
	void TreePopInspector();

	class InspectorRenderer {
	public:
		virtual std::string GetTypeName() const = 0;
		virtual int32_t GetPriority() const = 0;
		virtual std::string GetHeader() const = 0;

		virtual void Invoke(uint64_t goID) = 0;
		virtual void InvokeGizmo(uint64_t goID) = 0;

		virtual bool HasComponent(uint64_t goID) const = 0;
		virtual void RemoveComponent(uint64_t goID) const = 0;
	};

	class InspectorPanel : public Panel {
	public:
		InspectorPanel();
		~InspectorPanel() = default;

		virtual void DrawImGui() override;
		virtual void DrawImGuizmo() override;

		virtual void PreWindow() override;
		virtual void PostWindow() override;
		
		bool HasInspector(const std::string& typeName) const;
	private:
		std::vector<Stulu::Ref<InspectorRenderer>> m_inspectors;
		entt::entity m_nameEditing = entt::null;
		static Stulu::Mono::Method s_renderMethod;
		static Stulu::Mono::Method s_renderGizmosMethod;

		float m_windowIndent = 0.0f;

		void LoadObjects();
		void LoadScriptObjects(Stulu::Ref<Stulu::ScriptAssembly> assembly);

		friend class ManagedInspectorRenderer;
	};


	struct ManagedInspectorRendererProps {
		Stulu::Mono::ReflectionType refType = nullptr;
		Stulu::Ref<Stulu::ScriptAssembly> assembly;
		Stulu::Mono::Class inspectorClass = nullptr;
		Stulu::Mono::String header = nullptr;
		int32_t prio = -50;
	};
	class ManagedInspectorRenderer : public InspectorRenderer {
	public:
		ManagedInspectorRenderer(const ManagedInspectorRendererProps& props)
			: m_refType(props.refType), m_header(props.header.ToUtf8()), m_prio(props.prio),
			m_object(Stulu::createRef<Stulu::MonoObjectInstance>(props.inspectorClass, props.assembly.get())) {
		}

		virtual bool HasComponent(uint64_t goID) const override {
			return StuluBindings::GameObject::hasComponent(goID, m_refType);
		}
		virtual void RemoveComponent(uint64_t goID) const override {
			StuluBindings::GameObject::removeComponent(goID, m_refType);
		}
		virtual int32_t GetPriority() const override {
			return m_prio;
		}
		virtual std::string GetHeader() const override {
			return m_header;
		}
		virtual std::string GetTypeName() const override {
			return m_refType.GetType().GetNameFull(Stulu::Mono::TypeNameFormat::FULL_NAME);
		}
		virtual void Invoke(uint64_t goID) override {
			uint64_t id = (uint64_t)goID;
			void* args[1] = { &id };
			m_object->CallMethod(InspectorPanel::s_renderMethod, args);
		}
		virtual void InvokeGizmo(uint64_t goID) override {
			uint64_t id = (uint64_t)goID;
			void* args[1] = { &id };
			m_object->CallMethod(InspectorPanel::s_renderGizmosMethod, args);
		}
	private:
		Stulu::Mono::ReflectionType m_refType = nullptr;
		Stulu::Ref<Stulu::MonoObjectInstance> m_object;
		std::string m_header;
		int32_t m_prio;
	};

	template<class T>
	class NativeInspectorRenderer : public InspectorRenderer {
	public:
		virtual bool HasComponent(uint64_t goID) const override {
			return Stulu::GameObject((entt::entity)goID, GetActiveScene()).hasComponent<T>();
		}
		virtual void RemoveComponent(uint64_t goID) const override {
			Stulu::GameObject((entt::entity)goID, GetActiveScene()).removeComponent<T>();
		}
		virtual int32_t GetPriority() const override {
			return 1;
		}
		virtual std::string GetHeader() const override {
			return Stulu::Component::GetNativeComponentName<T>();
		}
		virtual std::string GetTypeName() const override {
			return Stulu::Component::GetManagedComponentName<T>();
		}
		virtual void Invoke(uint64_t goID) override {
			DrawGUI(Stulu::GameObject((entt::entity)goID, GetActiveScene()).getComponent<T>());
		}
		virtual void InvokeGizmo(uint64_t goID) override {
			DrawGuizmo(Stulu::GameObject((entt::entity)goID, GetActiveScene()).getComponent<T>());
		}

		virtual void DrawGUI(T& go) const {};
		virtual void DrawGuizmo(T& go) const {};
	protected:
		Stulu::Scene* GetActiveScene() const {
			return App::get().GetLayer().GetActiveScene().get();
		}
	};
}