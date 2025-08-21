#pragma once
#include <Stulu/Core/Module.h>

#include "ParticleSystemLayer.h"
#include "ParticleSystem.h"

namespace Stulu {
	class ParticleSystemModule : public Module {
	public:
		using ModuleType = ModuleDefault;

		ParticleSystemModule();
		~ParticleSystemModule();
		
		bool onLoad(const ApplicationInfo& appInfo) override;
		void onAttach() override;
	};
}