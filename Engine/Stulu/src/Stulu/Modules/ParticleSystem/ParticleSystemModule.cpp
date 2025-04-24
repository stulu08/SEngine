#include <st_pch.h>
#include "ParticleSystemModule.h"

namespace Stulu {
	ParticleSystemModule::ParticleSystemModule() 
		: Module("ParticleSystemModule") {

	}
	ParticleSystemModule::~ParticleSystemModule() {

	}
	void ParticleSystemModule::onLoad() {
		EventCaller::RegisterLayer<ParticleSystemLayer>();
	}
	void ParticleSystemModule::onAttach() {
		Component::Register<ParticleSystemComponent>("ParticleSystemComponent", "Stulu.ParticleSystemComponent");
	}
}