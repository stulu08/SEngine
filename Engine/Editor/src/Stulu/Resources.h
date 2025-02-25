#pragma once
#include <Stulu.h>

namespace Editor {
	class Resources {
	public:
		static Stulu::Ref<Stulu::Texture2D>& GetLogo();
		static Stulu::Ref<Stulu::Texture2D>& GetDirectoryIcon();
		static Stulu::Ref<Stulu::Texture2D>& GetFileIcon();
		static Stulu::Ref<Stulu::Texture2D>& GetSceneIcon();
		static Stulu::Ref<Stulu::Texture2D>& GetCodeIcon(const std::string& extension = "");

		static std::string GetIconsFont();
		static std::string GetFont();
		static std::string GetFontBold();
	};
}