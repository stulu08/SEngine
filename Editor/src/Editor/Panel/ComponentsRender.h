#pragma once
#include "../Resources.h"
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

		void addMaterial(const Ref<Material>& material);
		Ref<FrameBufferTexture>& getMaterial(const Ref<Material>& material);
		Ref<FrameBufferTexture>& getMaterial(const std::string& path);
		bool existsMaterial(const Ref<Material>& material);
		bool existsMaterial(const std::string& path);
	private:
		std::unordered_map<std::string, Ref<FrameBufferTexture>> materials;
		GameObject camera;
		GameObject cube;
		GameObject light;
		Ref<Scene> scene;

		void setUpScene(const Ref<Material>& material);

		inline static Previewing* s_instance = nullptr;
	};


	class ComponentsRender {
	public:
		template<typename T>
		static void drawComponent(GameObject gameObject, T& component);

		static bool drawStringControl(const std::string& header, std::string& v);
		static bool drawBoolControl(const std::string& header, bool& v);
		static bool drawIntControl(const std::string& header, int& v);
		static bool drawFloatControl(const std::string& header, float& v);
		static bool drawVector2Control(const std::string& header, glm::vec2& vec);
		static bool drawVector3Control(const std::string& header, glm::vec3& vec);
		static bool drawVector4Control(const std::string& header, glm::vec4& vec);
		static bool drawMat4Control(const std::string& header, glm::mat4& v);
		static bool drawTextureEdit(const std::string& header, Ref<Texture2D>& texture);
		static bool drawMaterialEdit(const std::string& header, Ref<Material>& material);
	};
}