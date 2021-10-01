#pragma once
#include <Stulu.h>

namespace Stulu {
	class Resources {
	public:
		static void loadAll();

		static Ref<Texture2D> getEmptyTextureTexture();
	private:
		static inline Ref<Texture2D> s_EmptyTextureTexture = nullptr;
	};
}