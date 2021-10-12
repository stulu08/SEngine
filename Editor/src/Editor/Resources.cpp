#include "Resources.h"

namespace Stulu {

	void Resources::loadAll() {
		s_emptyTexture = Texture2D::create("Stulu/assets/Textures/empty.png");
		s_fileTexture = Texture2D::create("Stulu/assets/Textures/file.png");
		s_folderTexture = Texture2D::create("Stulu/assets/Textures/folder.png");
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
}