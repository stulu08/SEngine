#pragma once
#include <Stulu.h>

namespace Editor {
	class Resources {
	public:
		static void ReleaseAll();

		static Stulu::Texture2D* GetLogo();
		static Stulu::Texture2D* GetDirectoryIcon();
		static Stulu::Texture2D* GetFileIcon();
		static Stulu::Texture2D* GetSceneIcon();
		static Stulu::Texture2D* GetCodeIcon(const std::string& extension = "");

		static std::string GetIconsFont();
		static std::string GetFont();
		static std::string GetFontBold();

		static Stulu::Ref<Stulu::Shader>& GetOutlineShader();
		static Stulu::Ref<Stulu::Shader>& GetHighliteShader();
		static Stulu::Ref<Stulu::Shader>& GetTransparentShader();

		static Stulu::Texture2D* GetLightTexture();
		static Stulu::Texture2D* GetDirectionalLightTexture();
		static Stulu::Texture2D* GetSpotLightTexture();
		static Stulu::Texture2D* GetCameraTexture();
	};
}