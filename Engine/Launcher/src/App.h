#include <Stulu.h>
#include "Layer.h"

namespace Stulu::Launcher {
	class LauncherApp : public Application {
	public:
		LauncherApp(const ApplicationInfo& info);
		virtual ~LauncherApp();

		inline static LauncherApp& get() { return *s_instance; }

	private:
		LauncherLayer* m_layer;
		static LauncherApp* s_instance;
	};
}