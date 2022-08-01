#pragma once
#include <Stulu.h>

#define getEditorApp() Stulu::EditorApp::get()
#define getEditorLayer() Stulu::EditorApp::getLayer()
#define getEditorScene() Stulu::EditorApp::getLayer().getActiveScene()
#define getEditorProject() Stulu::EditorApp::getProject()

namespace Stulu {
	class EditorResources {
	public:
		static Ref<Texture>& getEmptySlotTexture();
		static Ref<Texture>& getFileTexture();
		static Ref<Texture>& getFolderTexture();
		static Ref<Texture>& getObjectTexture();
		static Ref<Texture>& getSceneTexture();
		static Ref<Texture>& getNoCameraTexture();
		static Ref<Texture>& getPlayTexture();
		static Ref<Texture>& getPauseTexture();
		static Ref<Texture>& getStopTexture();
		static Ref<Texture>& getSkyBoxTexture();
		static Ref<Texture>& getScriptTexture();

		static Ref<Texture>& getLightTexture();
		static Ref<Texture>& getDirectionalLightTexture();
		static Ref<Texture>& getSpotLightTexture();
		static Ref<Texture>& getCameraTexture();

		static const std::string getDefaultSceneSource();

		static Ref<Shader>& getOutlineShader();
		static Ref<Shader>& getGreenColorShader();
		static Ref<Shader>& getTransparentShader();
	};
}