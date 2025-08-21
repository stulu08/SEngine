#pragma once
#include "Stulu/Controls.h"
#include <Stulu/Scripting/Managed/Bindings/Bindings.h>
#include <Stulu/Panels/Inspector.h>
namespace StuluBindings {
	class ImGui {
	public:
		static inline void Text(Stulu::Mono::String text) {
			::Editor::Controls::Text(text.ToUtf8().c_str());
		}
		static inline bool Vector2(Stulu::Mono::String monoName, struct Vector2* value, float speed) {
			glm::vec2* valuePtr = (glm::vec2*)value;
			return ::Editor::Controls::Vector2(monoName.ToUtf8(), *valuePtr, speed);
		}
		static inline bool Vector3(Stulu::Mono::String monoName, struct Vector3* value, float speed) {
			glm::vec3* valuePtr = (glm::vec3*)value;
			return ::Editor::Controls::Vector3(monoName.ToUtf8(), *valuePtr, speed);
		}
		static inline bool Float(Stulu::Mono::String monoName, float* value, float min, float max, float speed) {
			return ::Editor::Controls::Float(monoName.ToUtf8(), *value, min, max, speed);
		}
		static inline bool Int(Stulu::Mono::String monoName, int32_t* value, int32_t min, int32_t max, float speed) {
			return ::Editor::Controls::Int(monoName.ToUtf8(), *value, min, max, speed);
		}
		static inline bool UInt(Stulu::Mono::String monoName, uint32_t* value, uint32_t min, uint32_t max, float speed) {
			return ::Editor::Controls::UInt(monoName.ToUtf8(), *value, min, max, speed);
		}
		static inline bool Bool(Stulu::Mono::String monoName, bool* value) {
			return ::Editor::Controls::Bool(monoName.ToUtf8(), *value);
		}
		static inline bool TreeBegin(Stulu::Mono::String monoName, uint32_t flags) {
			return ::ImGui::TreeNodeEx(monoName.ToUtf8().c_str(), (ImGuiTreeNodeFlags)flags);
		}
		static inline bool InspectorTreeBeginIndent(Stulu::Mono::String monoName, bool* outClose, Stulu::Mono::String closeText) {
			return ::Editor::TreeNodeInspector(monoName.ToUtf8(), outClose, closeText.ToUtf8());
		}
		static inline void InspectorTreePopunindent(Stulu::Mono::String monoName, uint32_t flags) {
			::Editor::TreePopInspector();
		}
		static inline bool Combo(Stulu::Mono::String monoName, int32_t& value, Stulu::Mono::String monoNames) {
			return ::Editor::Controls::Combo(monoName.ToUtf8(), value, monoNames.ToUtf8());
		}
		static inline void PushID(Stulu::Mono::String id) {
			::ImGui::PushID(id.ToUtf8().c_str());
		}
		static inline bool Button(Stulu::Mono::String name) {
			return ::ImGui::Button(name.ToUtf8().c_str());
		}
		static inline bool Texture2D(Stulu::Mono::String name, uint64_t* assetID) {
			Stulu::UUID uuid = *assetID;
			if (::Editor::Controls::Texture2D(name.ToUtf8(), uuid)) {
				*assetID = uuid;
				return true;
			}
			return false;
		}
		static inline bool Material(Stulu::Mono::String name, uint64_t* assetID) {
			Stulu::UUID uuid = *assetID;
			if (::Editor::Controls::Material(name.ToUtf8(), uuid)) {
				*assetID = uuid;
				return true;
			}
			return false;
		}
		static inline bool Mesh(Stulu::Mono::String name, uint64_t* assetID) {
			Stulu::UUID uuid = *assetID;
			if (::Editor::Controls::Mesh(name.ToUtf8(), uuid)) {
				*assetID = uuid;
				return true;
			}
			return false;
		}
		static inline bool GameObject(Stulu::Mono::String name, uint64_t* goID) {
			Stulu::Registry* registry = StuluBindings::GetCurrentRegistry();
			entt::entity id = (entt::entity)(*goID);
			Stulu::GameObject gameObject = { id, registry };
			
			std::string objectName = "GameObject";
			if (GameObjectBaseComponent* component = gameObject.tryGetComponent<GameObjectBaseComponent>()) {
				objectName = component->name;
			}

			if (::Editor::Controls::GameObjectFrame(name.ToUtf8(), id, objectName, registry)) {
				*goID = (uint64_t)id;
				return true;
			}
			return false;
		}
	};
}