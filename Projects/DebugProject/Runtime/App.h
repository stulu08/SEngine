#include <Stulu.h>
#include "Layer.h"

namespace Stulu {
	class RuntimeApp : public Application {
	public:
		RuntimeApp(const ApplicationInfo& appInfo);
		virtual ~RuntimeApp();
	};
}