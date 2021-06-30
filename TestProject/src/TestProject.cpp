#include <Stulu.h>
 
class TestProject : public Stulu::Application {
public:
	TestProject() {
		ST_INFO("Reached TestProject");
	}
	~TestProject() {

	}
};

Stulu::Application* Stulu::CreateApplication() {
	return new TestProject();
}