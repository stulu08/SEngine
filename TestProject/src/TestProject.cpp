#include <Stulu.h>

class TestProject : public Stulu::Application {
public:
	TestProject() {

	}
	~TestProject() {

	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}