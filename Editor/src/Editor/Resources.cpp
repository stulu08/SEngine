#include "Resources.h"

namespace Stulu {

	void EditorResources::loadAll() {
		s_emptyTexture = Texture2D::create("Stulu/assets/Textures/Icons/empty.png");
		s_fileTexture = Texture2D::create("Stulu/assets/Textures/Icons/file.png");
		s_folderTexture = Texture2D::create("Stulu/assets/Textures/Icons/folder.png");
		s_objectTexture = Texture2D::create("Stulu/assets/Textures/Icons/object.png");
		s_sceneTexture = Texture2D::create("Stulu/assets/Textures/Icons/scene.png");
		s_noCameraTexture = Texture2D::create("Stulu/assets/Textures/Icons/nocam.png");
		s_playTexture = Texture2D::create("Stulu/assets/Textures/Icons/play.png");
		s_pauseTexture = Texture2D::create("Stulu/assets/Textures/Icons/pause.png");
		s_stopTexture = Texture2D::create("Stulu/assets/Textures/Icons/stop.png");
		s_skyboxTexture = Texture2D::create("Stulu/assets/Textures/Icons/skybox.png");
	}
	Ref<Texture>& EditorResources::getEmptySlotTexture() {
		return s_emptyTexture;
	}
	Ref<Texture>& EditorResources::getFileTexture() {
		return s_fileTexture;
	}
	Ref<Texture>& EditorResources::getFolderTexture() {
		return s_folderTexture;
	}
	Ref<Texture>& EditorResources::getObjectTexture() {
		return s_objectTexture;
	}
	Ref<Texture>& EditorResources::getSceneTexture() {
		return s_sceneTexture;
	}
	Ref<Texture>& EditorResources::getNoCameraTexture() {
		return s_noCameraTexture;
	}
	Ref<Texture>& EditorResources::getPlayTexture() {
		return s_playTexture;
	}
	Ref<Texture>& EditorResources::getPauseTexture() {
		return s_pauseTexture;
	}
	Ref<Texture>& EditorResources::getStopTexture() {
		return s_stopTexture;
	}
	Ref<Texture>& EditorResources::getSkyBoxTexture() {
		return s_skyboxTexture;
	}
}