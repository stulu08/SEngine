#include <Stulu.h>
#include "Core.h"
#include "Component/FreeCamera.h"

class APP_API ProjectLayer : public Stulu::SceneLayer {
public:
    virtual bool Initlize(Stulu::Scene* scene) override {
        return true;
    }

};

class APP_API MyProject : public Stulu::Module {
public:
	virtual void onAttach() override {
        Stulu::EventCaller::RegisterLayer<ProjectLayer>();
        Stulu::Component::Register<FreeCamera>("FreeCameraComponent", "FreeCameraComponent");
	}
	virtual void onUpdate(Stulu::Timestep ts) override {
		
	}
	virtual void onEvent(Stulu::Event& e) override {
		
	}
};

namespace Loader {
    static MyProject* Instance;

    static bool Attach() {
        if (Instance) {
            CORE_ERROR("Instance has already been created!");
            return false;
        }
        Stulu::Application::LoadModule<MyProject>();
        return true;
    }
    static bool Detach() {
        if (!Instance) {
            CORE_ERROR("Instance has not been created!");
            return false;
        }
        Stulu::Application::get().popLayer(Instance);
        delete Instance;
        return true;
    }
}


#ifdef ST_WINDOWS
BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,  // handle to DLL module
    DWORD fdwReason,     // reason for calling function
    LPVOID lpReserved)  // reserved
{
    // Perform actions based on the reason for calling.
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        return Loader::Attach() ? TRUE : FALSE;
        break;

    case DLL_THREAD_ATTACH:
        // Do thread-specific initialization.
        break;

    case DLL_THREAD_DETACH:
        // Do thread-specific cleanup.
        break;

    case DLL_PROCESS_DETACH:
        return Loader::Detach() ? TRUE : FALSE;
        break;
    }
    return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif