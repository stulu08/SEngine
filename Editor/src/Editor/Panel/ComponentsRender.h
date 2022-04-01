#pragma once
#include "Editor/Resources.h"
#include <imgui/imgui.h>
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
		GameObject sphere;
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
		static bool drawBoolControl(const std::string& header, bool& v);
		static bool drawIntControl(const std::string& header, int& v);
		static bool drawIntSliderControl(const std::string& header, int& v, int min = 0, int max = 10);
		static bool drawFloatControl(const std::string& header, float& v);
		static bool drawFloatSliderControl(const std::string& header, float& v, float min = 0.0f, float max = 1.0f);
		static bool drawVector2Control(const std::string& header, glm::vec2& vec);
		static bool drawVector3Control(const std::string& header, glm::vec3& vec);
		static bool drawVector4Control(const std::string& header, glm::vec4& vec);
		static bool drawComboControl(const std::string& header, int& current_item, const char* items_separated_by_zeros, int height_in_items = -1);
		static bool drawComboControl(const std::string& header, int& current_item, const std::vector<std::string>& items);
		static bool drawMat4Control(const std::string& header, glm::mat4& v);
		static void drawHelpMarker(const char* desc);
		static bool drawTextureEdit(const std::string& header, UUID& uuid, AssetType type = AssetType::Texture2D);
		static bool drawMaterialEdit(const std::string& header, UUID& uuid, bool canChange = true);
		static bool drawMeshEdit(const std::string& header, UUID& uuid);
	};
}