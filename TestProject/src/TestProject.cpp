#include "TestProject.h"
#include <Stulu/EntryPoint.h>


Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}