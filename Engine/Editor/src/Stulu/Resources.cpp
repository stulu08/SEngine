#include "Resources.h"
#include "App.h"

using namespace Stulu;

namespace Editor {
	Ref<Stulu::Texture2D>& Resources::GetLogo() {
		static Ref<Texture2D> texture = Texture2D::create(App::GetEditorDataPath() + "/Icons/Logo.png");
		return texture;
	}
}