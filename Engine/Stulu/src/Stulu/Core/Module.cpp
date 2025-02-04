#include "st_pch.h"
#include "Module.h"

#include "Stulu/Core/Application.h"
#include "Stulu/Modules/ParticleSystem/ParticleSystemModule.h"

namespace Stulu {
	void Module::LoadBaseModules() {
		Application::AddModule<ParticleSystemModule>();
	}
}

