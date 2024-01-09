#pragma once
#include "Editor/Resources.h"
#include <imgui/imgui.h>
#define MAGIC_ENUM_RANGE_MIN 0
#define MAGIC_ENUM_RANGE_MAX 256
#include <magic_enum/magic_enum.hpp>
namespace Stulu {
	class Previewing {
	public:
		inline static void init() {
			s_instance = new Previewing();
		}
		Previewing();
		inline static Previewing& get() {
			return *s_instance;
		}
		void add(Asset& asset);
		Ref<Texture>& get(const UUID& uuid);
		bool exists(const UUID& uuid);
	private:
		std::unordered_map<UUID, Ref<Texture>> items;
		GameObject camera;
		GameObject renderObject;
		GameObject light;
		Ref<Scene> scene;

		void setUpScene(Asset& asset);

		inline static Previewing* s_instance = nullptr;
	};


	class ComponentsRender {
	public:
		template<typename T>
		static void drawComponent(GameObject gameObject, T& component);

		static bool drawStringControl(const std::string& header, std::string& v);
		//returns true if changed
		static bool drawBoolControl(const std::string& header, bool& v);
		static bool drawBoolBitFlagControl(const std::string& header, uint32_t flag, uint32_t& flags);
		static bool drawIntControl(const std::string& header, int& v);
		static bool drawInt3Control(const std::string& header, int& x, int& y, int& z);
		static bool drawIntSliderControl(const std::string& header, int& v, int min = 0, int max = 10);
		static bool drawUIntControl(const std::string& header, uint32_t& v, uint32_t min = 0, uint32_t max = 0);
		static bool drawUIntSliderControl(const std::string& header, uint32_t& v, uint32_t min = 0, uint32_t max = 10);
		static bool drawFloatControl(const std::string& header, float& v, float min = .0f, float max = .0f, float speed = .1f);
		static bool drawFloatSliderControl(const std::string& header, float& v, float min = 0.0f, float max = 1.0f);
		static bool drawVector2Control(const std::string& header, glm::vec2& vec);
		static bool drawVector3Control(const std::string& header, glm::vec3& vec);
		static bool drawVector4Control(const std::string& header, glm::vec4& vec);
		static bool drawVector4Control(const std::string& header, glm::quat& quat);
		static bool drawComboControl(const std::string& header, int32_t& current_item, const char* items_separated_by_zeros, int height_in_items = -1);
		static bool drawComboControl(const std::string& header, int32_t& current_item, const std::vector<std::string>& items);
		static bool drawMat4Control(const std::string& header, glm::mat4& v);
		static bool drawHDRColorEdit(const std::string& header, glm::vec4& color);
		static bool drawAssetControl(const std::string& header, UUID& uuid, AssetType type);


		template<typename E>
		static bool drawEnumComboControl(const std::string& header, E& current_item) {
			constexpr auto items = magic_enum::enum_names<E>();
			std::vector<std::string> names(items.size());
			for (int i = 0; i < items.size(); i++) {
				names[i] = items[i].data();
			}
			int32_t it = (int32_t)current_item;
			bool re = drawComboControl(header, it, names);
			current_item = (E)it;
			return re;
		}

		static void drawHelpMarker(const char* desc);
		static bool drawTextureEdit(const std::string& header, UUID& uuid, bool cubeMap = false);
		static bool drawGameObjectEdit(const std::string& header, UUID& gameObject);
		static bool drawMaterialEdit(const std::string& header, UUID& uuid, bool canChange = true);
		static bool drawMeshEdit(const std::string& header, UUID& uuid);

		template<typename T>
		static bool drawControl(const std::string& header, T& value) {}
		template<>
		static bool drawControl(const std::string& header, float& value) { return drawFloatControl(header, value); }
		template<>
		static bool drawControl(const std::string& header, int32_t& value) { return drawIntControl(header, value); }
		template<>
		static bool drawControl(const std::string& header, uint32_t& value) { return drawUIntControl(header, value); }
		template<>
		static bool drawControl(const std::string& header, std::string& value) { return drawStringControl(header, value); }
		template<>
		static bool drawControl(const std::string& header, glm::vec2& value) { return drawVector2Control(header, value); }
		template<>
		static bool drawControl(const std::string& header, glm::vec3& value) { return drawVector3Control(header, value); }
		template<>
		static bool drawControl(const std::string& header, glm::vec4& value) { return drawVector4Control(header, value); }
		template<>
		static bool drawControl(const std::string& header, glm::mat4& value) { return drawMat4Control(header, value); }
		template<>
		static bool drawControl(const std::string& header, bool& value) { return drawBoolControl(header, value); }
	};
}