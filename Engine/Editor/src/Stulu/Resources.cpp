#include "Resources.h"
#include "App.h"

using namespace Stulu;

namespace Editor {
	static struct EdtitorResourceStorage {
		Ref<Texture2D> LogoTexture;
		Ref<Texture2D> DirectoryTexture;
		Ref<Texture2D> FileTexture;
		Ref<Texture2D> SceneTextuer;
		Ref<Texture2D> CodeTexture;
		Ref<Texture2D> LightTexture;
		Ref<Texture2D> DirLightTexture;
		Ref<Texture2D> SpotlighTexture;
		Ref<Texture2D> CameraTexture;
		std::unordered_map<std::string, Ref<Texture2D>> CodeTextureCache;

		Ref<Shader> OutlineShader;
		Ref<Shader> TransparentShader;
		Ref<Shader> HighliteShader;
	} s_storage;

	void Resources::ReleaseAll() {
		s_storage = EdtitorResourceStorage();
	}

	Texture2D* Resources::GetLogo() {
		if (!s_storage.LogoTexture) {
			s_storage.LogoTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Logo.png");
		}
		return s_storage.LogoTexture.get();
	}
	Texture2D* Resources::GetDirectoryIcon() {
		if(!s_storage.DirectoryTexture) {
			s_storage.DirectoryTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Folder.png");
		}
		return s_storage.DirectoryTexture.get();
	}
	Texture2D* Resources::GetFileIcon() {
		if (!s_storage.FileTexture) {
			s_storage.FileTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/File.png");
		}
		return s_storage.FileTexture.get();
	}
	Texture2D* Resources::GetSceneIcon() {
		if (!s_storage.SceneTextuer) {
			s_storage.SceneTextuer = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/Scene.png");
		}
		return s_storage.SceneTextuer.get();
	}
	Texture2D* Resources::GetCodeIcon(const std::string& lowerExtension) {
		auto& m_cache = s_storage.CodeTextureCache;

		if (!s_storage.CodeTexture) {
			s_storage.CodeTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/Code.png");
		}

		if (lowerExtension.empty())
			return s_storage.CodeTexture.get();

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

		return s_storage.CodeTexture.get();
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
		if (!s_storage.OutlineShader) {
			s_storage.OutlineShader = Renderer::getShaderSystem()->GetShader("Editor/Outline");
		}
		return s_storage.OutlineShader;
	}
	Ref<Shader>& Resources::GetHighliteShader() {
		if (!s_storage.HighliteShader) {
			s_storage.HighliteShader = Renderer::getShaderSystem()->GetShader("Editor/Green");
		}
		return s_storage.HighliteShader;
	}
	Stulu::Ref<Stulu::Shader>& Resources::GetTransparentShader() {
		if (!s_storage.TransparentShader) {
			s_storage.TransparentShader = Renderer::getShaderSystem()->GetShader("Editor/Transparent");
		}
		return s_storage.TransparentShader;
	}

	Texture2D* Resources::GetDirectionalLightTexture() {
		if(!s_storage.DirLightTexture) {
			s_storage.DirLightTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Directionallight.png");
		}
		return s_storage.DirLightTexture.get();
	}
	Texture2D* Resources::GetLightTexture() {
		if (!s_storage.LightTexture) {
			s_storage.LightTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Light.png");
		}
		return s_storage.LightTexture.get();
	}
	Texture2D* Resources::GetSpotLightTexture() {
		if (!s_storage.SpotlighTexture) {
			s_storage.SpotlighTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Spotlight.png");
		}
		return s_storage.SpotlighTexture.get();
	}
	Texture2D* Resources::GetCameraTexture() {
		if (!s_storage.CameraTexture) {
			s_storage.CameraTexture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Gizmo/Camera.png");
		}
		return s_storage.CameraTexture.get();
	}
}