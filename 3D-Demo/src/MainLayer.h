#pragma once
#include <World.h>
using namespace Stulu;

class MainLayer : public Layer {
public:
	MainLayer()
		: Layer("Main"), m_cam(new CamerControllerSetting()),m_world() {

	}
	~MainLayer() {

	}

	virtual void onAttach() override;
	virtual void onUpdate(Timestep time) override;
	virtual void onEvent(Event& e) override;


private:
	CameraController m_cam;
	World m_world;
};

