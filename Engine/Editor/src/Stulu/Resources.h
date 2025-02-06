#include <Stulu.h>

namespace Editor {
	class Resources {
	public:
		static Stulu::Ref<Stulu::Texture2D>& GetLogo();

		static std::string GetIconsFont();
		static std::string GetFont();
	};
}