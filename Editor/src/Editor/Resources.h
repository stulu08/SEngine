#pragma once
#include <Stulu.h>

namespace Stulu {
	class Resources {
	public:
		static void loadAll();

		static Ref<Texture2D> getEmptySlotTexture();
		static Ref<Texture2D> getFileTexture();
		static Ref<Texture2D> getFolderTexture();
	private:
		static inline Ref<Texture2D> s_emptyTexture = nullptr;
		static inline Ref<Texture2D> s_fileTexture = nullptr;
		static inline Ref<Texture2D> s_folderTexture = nullptr;
	};
}