#include <st_pch.h>
#include "ParticleSystemModule.h"

namespace Stulu {
	ParticleSystemModule::ParticleSystemModule() 
		: Module("ParticleSystemModule") {

	}
	ParticleSystemModule::~ParticleSystemModule() {

	}
	bool ParticleSystemModule::onLoad(const ApplicationInfo& appInfo) {
		EventCaller::RegisterLayer<ParticleSystemLayer>();
		return true;
	}
	void ParticleSystemModule::onAttach() {
		Component::Register<ParticleSystemComponent>("ParticleSystemComponent", "Stulu.ParticleSystemComponent");
	}
}