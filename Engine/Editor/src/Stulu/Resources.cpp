#include "Resources.h"
#include "App.h"

using namespace Stulu;

namespace Editor {
	Ref<Stulu::Texture2D>& Resources::GetLogo() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Logo.png");
		return texture;
	}
	Stulu::Ref<Stulu::Texture2D>& Resources::GetDirectoryIcon() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Folder.png");
		return texture;
	}
	Stulu::Ref<Stulu::Texture2D>& Resources::GetFileIcon() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/File.png");
		return texture;
	}
	Stulu::Ref<Stulu::Texture2D>& Resources::GetSceneIcon() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/Scene.png");
		return texture;
	}
	Stulu::Ref<Stulu::Texture2D>& Resources::GetCodeIcon(const std::string& lowerExtension) {
		static std::unordered_map<std::string, Ref<Texture2D>> m_cache;
		static Ref<Texture2D> code_texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Files/Code.png");

		if (lowerExtension.empty())
			return code_texture;

		std::string extension = lowerExtension;
		if (extension[0] == '.')
			extension.erase(0, 1);
		std::transform(extension.begin(), extension.end(), extension.begin(), ::toupper);

		if (m_cache.find(extension) != m_cache.end()) {
			auto& texture = m_cache.at(extension);
			if(texture)
				return texture;
		}
		
		const std::string file = App::GetEditorDataPath() + "/Icons/Files/" + extension + ".png";
		if (std::filesystem::exists(file)) {
			auto texture = Texture2D::create(file);
			if (texture) {
				m_cache.insert({ extension, texture });
				return texture;
			}
		}

		return code_texture;
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
}