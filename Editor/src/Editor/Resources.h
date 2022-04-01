#pragma once
#include <Stulu.h>

#define getEditorApp() Stulu::EditorApp::get()
#define getEditorLayer() Stulu::EditorApp::getLayer()
#define getEditorScene() ::Stulu::EditorApp::getLayer().getActiveScene()
#define getEditorProject() Stulu::EditorApp::getProject()

namespace Stulu {
	class EditorResources {
	public:
		static void loadAll();

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
		static const std::string getDefaultSceneSource();

		static Ref<Shader>& getOutlineShader();
	private:
		static inline Ref<Texture> s_emptyTexture = nullptr;
		static inline Ref<Texture> s_fileTexture = nullptr;
		static inline Ref<Texture> s_folderTexture = nullptr;
		static inline Ref<Texture> s_objectTexture = nullptr;
		static inline Ref<Texture> s_sceneTexture = nullptr;
		static inline Ref<Texture> s_noCameraTexture = nullptr;
		static inline Ref<Texture> s_playTexture = nullptr;
		static inline Ref<Texture> s_pauseTexture = nullptr;
		static inline Ref<Texture> s_stopTexture = nullptr;
		static inline Ref<Texture> s_skyboxTexture = nullptr;

		static inline Ref<Shader> s_outlineShader = nullptr;
	};
}