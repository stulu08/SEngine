#include "Resources.h"

namespace Stulu {

	void Resources::loadAll() {
		s_EmptyTextureTexture = Texture2D::create("Stulu/assets/Textures/empty.png");
	}
	Ref<Texture2D> Resources::getEmptyTextureTexture() {
		return s_EmptyTextureTexture;
	}
}