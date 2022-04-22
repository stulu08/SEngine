#include <Stulu.h>
#include "Layer.h"

namespace Stulu {
	class RuntimeApp : public Application {
	public:
		RuntimeApp(const ApplicationInfo& appInfo);
		virtual ~RuntimeApp();

		inline static RuntimeApp& get() { return *s_instance; }
		inline static RuntimeLayer& getLayer() { return *get().editorLayer; }
	private:
		RuntimeLayer* editorLayer;
		static RuntimeApp* s_instance;
	};
}