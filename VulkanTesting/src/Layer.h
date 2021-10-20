#include <Stulu.h>

using namespace Stulu;

class DefaultLayer : public Layer {
public:
	DefaultLayer()
		: Layer("Default") {
	}
	virtual void onAttach() override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImguiRender(Timestep ts) override;
	virtual void onEvent(Event& e) override;


private:
	Ref<Shader> shader;
};
