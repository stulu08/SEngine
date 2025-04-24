#include "Resources.h"
#include "App.h"

using namespace Stulu;

namespace Editor {
	Texture2D* Resources::GetLogo() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Logo.png");
		return texture.get();
	}
	Texture2D* Resources::GetDirectoryIcon() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Folder.png");
		return texture.get();
	}
	Texture2D* Resources::GetFileIcon() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/File.png");
		return texture.get();
	}
	Texture2D* Resources::GetSceneIcon() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/Scene.png");
		return texture.get();
	}
	Texture2D* Resources::GetCodeIcon(const std::string& lowerExtension) {
		static std::unordered_map<std::string, Ref<Texture2D>> m_cache;
		static Ref<Texture2D> code_texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/Code.png");

		if (lowerExtension.empty())
			return code_texture.get();

		std::string extension = lowerExtension;
		if (extension[0] == '.')
			extension.erase(0, 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);

		if (m_cache.find(extension) != m_cache.end()) {
			auto& texture = m_cache.at(extension);
			if(texture)
				return texture.get();
		}
		
		const std::string file = App::GetEditorDataPath() + "/Icons/Files/" + extension + ".png";
		if (std::filesystem::exists(file)) {
			auto texture = Texture2D::create(file);
			if (texture) {
				m_cache.insert({ extension, texture });
				return texture.get();
			}
		}

		return code_texture.get();
	}
	
	std::string Resources::GetIconsFont() {
		return App::GetEditorDataPath() + "/Fonts/EditorIcons.ttf";
	}
	std::string Resources::GetFont() {
		return App::GetEditorDataPath() + "/Fonts/EditorFont.ttf";
	}
	std::string Resources::GetFontBold() {
		return App::GetEditorDataPath() + "/Fonts/EditorFontBold.otf";
	}
	Ref<Shader>& Resources::GetOutlineShader() {
		static Ref<Shader> shader;
		if (!shader) {
			shader = Renderer::getShaderSystem()->GetShader("Editor/Outline");
		}
		return shader;
	}
	Ref<Shader>& Resources::GetHighliteShader() {
		static Ref<Shader> shader;
		if (!shader) {
			shader = Renderer::getShaderSystem()->GetShader("Editor/Green");
		}
		return shader;
	}
	Stulu::Ref<Stulu::Shader>& Resources::GetTransparentShader() {
		static Ref<Shader> shader;
		if (!shader) {
			shader = Renderer::getShaderSystem()->GetShader("Editor/Transparent");
		}
		return shader;
	}

	Texture2D* Resources::GetDirectionalLightTexture() {
		static Ref<Texture2D> s_texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Directionallight.png");
		return s_texture.get();
	}
	Texture2D* Resources::GetLightTexture() {
		static Ref<Texture2D> s_texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Light.png");
		return s_texture.get();
	}
	Texture2D* Resources::GetSpotLightTexture() {
		static Ref<Texture2D> s_texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Spotlight.png");
		return s_texture.get();
	}
	Texture2D* Resources::GetCameraTexture() {
		static Ref<Texture2D> s_texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Camera.png");
		return s_texture.get();
	}
}