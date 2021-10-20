#include "Resources.h"

namespace Stulu {

	void Resources::loadAll() {
		s_emptyTexture = Texture2D::create("Stulu/assets/Textures/Icons/empty.png");
		s_fileTexture = Texture2D::create("Stulu/assets/Textures/Icons/file.png");
		s_folderTexture = Texture2D::create("Stulu/assets/Textures/Icons/folder.png");
		s_objectTexture = Texture2D::create("Stulu/assets/Textures/Icons/object.png");
		s_sceneTexture = Texture2D::create("Stulu/assets/Textures/Icons/scene.png");
	}
	Ref<Texture2D> Resources::getEmptySlotTexture() {
		return s_emptyTexture;
	}
	Ref<Texture2D> Resources::getFileTexture() {
		return s_fileTexture;
	}
	Ref<Texture2D> Resources::getFolderTexture() {
		return s_folderTexture;
	}
	Ref<Texture2D> Resources::getObjectTexture() {
		return s_objectTexture;
	}
	Ref<Texture2D> Resources::getSceneTexture() {
		return s_sceneTexture;
	}
}