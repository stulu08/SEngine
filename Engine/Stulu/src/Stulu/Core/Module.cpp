#include "st_pch.h"
#include "Module.h"

#include "Stulu/Core/Application.h"

#include "Stulu/Modules/ParticleSystem/ParticleSystemModule.h"
#include "Stulu/Physics/PhysicsModule.h"

namespace Stulu {
	void Module::LoadBaseModules() {
		Application::AddModule<PhysicsModule>();
		Application::AddModule<ParticleSystemModule>();
	}
}

