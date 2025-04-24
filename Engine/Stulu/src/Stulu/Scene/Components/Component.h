#pragma once
#include "Stulu/Scene/GameObject.h"
#include "Stulu/Scene/Scene.h"
#include "Stulu/Core/Application.h"
#include "Stulu/Serialization/YAML.h"

namespace Stulu {
	class STULU_API Component {
	public:
		inline Component() {

		}
		inline Component(const Component& other) {
			this->gameObject = other.gameObject;
		}

		virtual ~Component() = default;

		//this GameObject
		GameObject gameObject = GameObject::null;

		virtual void onComponentAdded(Registry* scene) {};
		virtual void onComponentRemove(Registry* scene) {};

		virtual void Serialize(YAML::Emitter& out) const {};
		virtual void Deserialize(YAML::Node& node) {};

		template<class T>
		static inline void RegisterNative(const std::string& nativeName) {
			m_componentTable[typeid(T).hash_code()] = nativeName;
			m_componentCopyList[typeid(T).hash_code()] = CopyComponent<T>;
			m_componentSerializeList[typeid(T).hash_code()] = SerializeComponent<T>;
			m_componentDeserializeList[typeid(T).hash_code()] = DeserializeComponent<T>;
		}
		template<class T>
		static inline void RegisterManaged(const std::string& managedName) {
			Application::get().getAssemblyManager()->RegisterComponent<T>(managedName);
		}
		template<class T>
		static inline void Register(const std::string& nativeName, const std::string& managedName) {
			RegisterNative<T>(nativeName);
			RegisterManaged<T>(managedName);
		}

		static void RegisterBaseComponents();

		template<class T>
		static inline const std::string& GetNativeComponentName() {
			return m_componentTable.at(typeid(T).hash_code());
		}
	private:
		static std::unordered_map<size_t, std::function<void(entt::registry&, entt::registry&, Scene*)>> m_componentCopyList;
		static std::unordered_map<size_t, std::function<void(YAML::Emitter&,GameObject)>> m_componentSerializeList;
		static std::unordered_map<size_t, std::function<void(YAML::Node&,GameObject)>> m_componentDeserializeList;
		static std::unordered_map<size_t, std::string> m_componentTable;

		template<class T>
		static inline void CopyComponent(entt::registry& dst, entt::registry& src, Scene* scene) {
			auto view = src.view<T>();
			for (auto srcGameObject : view)
			{
				// they have the same entt id
				entt::entity dstGameObject = srcGameObject;
				T& srcComponent = src.get<T>(srcGameObject);
				
				// so this->gameObject can be used inside the copy constructor
				GameObject oldGameObject = srcComponent.gameObject;
				GameObject newGameObject = GameObject(dstGameObject, scene);
				srcComponent.gameObject = newGameObject;

				dst.emplace_or_replace<T>(dstGameObject, srcComponent).gameObject = newGameObject;

				// set the GameObject back
				srcComponent.gameObject = oldGameObject;
			}
		}
		template<class T>
		static inline void SerializeComponent(YAML::Emitter& out, GameObject gameObject) {
			if (gameObject.hasComponent<T>()) {
				const T& component = gameObject.getComponent<T>();
				out << YAML::Key << GetNativeComponentName<T>();
				out << YAML::BeginMap;
					component.Serialize(out);
				out << YAML::EndMap;
			}
		}
		template<class T>
		static inline void DeserializeComponent(YAML::Node& node, GameObject gameObject) {
			YAML::Node componentNode = node[GetNativeComponentName<T>()];
			if (componentNode) {
				T& component = gameObject.saveAddComponent<T>();
				component.Deserialize(componentNode);
			}
		}

		friend class Scene;
		friend class RegistrySerializer;
	};

	class GameObjectBaseComponent : public Component {
	public:
		std::string name = "GameObject";
		std::string tag = "Default";

		GameObjectBaseComponent() = default;
		GameObjectBaseComponent(const GameObjectBaseComponent&) = default;
		GameObjectBaseComponent(const std::string& name)
			: name(name) {};

		virtual void Serialize(YAML::Emitter& out) const override {
			out << YAML::Key << "name" << YAML::Value << name;
			out << YAML::Key << "tag" << YAML::Value << tag;
		}
		virtual void Deserialize(YAML::Node& node) override {
			if (node["name"])
				name = node["name"].as<std::string>();
			if (node["tag"])
				name = node["tag"].as<std::string>();
		}
	};
	// defined here to keep the function inline and prevent Declaraton problems
	inline bool GameObject::IsValid() const {
		if (m_entity == entt::null || m_registry == nullptr)
			return false;
		return hasComponent<GameObjectBaseComponent>();
	}
}