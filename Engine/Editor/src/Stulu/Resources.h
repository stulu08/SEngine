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

		static Stulu::Ref<Stulu::Shader>& GetOutlineShader();
		static Stulu::Ref<Stulu::Shader>& GetHighliteShader();
		static Stulu::Ref<Stulu::Shader>& GetTransparentShader();


		static Stulu::Ref<Stulu::Texture>& GetLightTexture();
		static Stulu::Ref<Stulu::Texture>& GetDirectionalLightTexture();
		static Stulu::Ref<Stulu::Texture>& GetSpotLightTexture();
		static Stulu::Ref<Stulu::Texture>& GetCameraTexture();
	};
}