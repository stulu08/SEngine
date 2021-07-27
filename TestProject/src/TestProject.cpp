#include "TestProject.h"
#include <Stulu/Core/EntryPoint.h>


Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}