#pragma once
#include "Stulu/Core/Platform.h"

namespace Stulu {
	class PlatformWindows : public Platform
	{
	protected:
		virtual float impl_getTime() override;
	};
}
