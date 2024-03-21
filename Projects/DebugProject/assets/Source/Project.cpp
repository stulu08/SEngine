#include <Stulu.h>
#include "Component/FreeCamera.h"

class MyProject : public Stulu::Layer {
public:
	virtual void onAttach() override {
		auto& assembly = Stulu::Application::get().getAssemblyManager();

		assembly->RegisterComponent<FreeCamera>("FreeCameraComponent");
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
        Instance = new MyProject();
        Stulu::Application::get().pushLayer(Instance);
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