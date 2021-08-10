#pragma once
#include <Stulu.h>
using namespace Stulu;

class Player {
public:
	Player();
	void update(Timestep ts);
	void draw();

	OrthographicCamera& getCam() { return m_cameraController.getCamera(); }


private:
	Transform m_transform;
	OrthographicCameraController m_cameraController;

	Ref<Texture2D> m_texture;

	float m_drag = -1.0f;
	float m_force = 2.0f;
	float m_rotationChangeDrag = -90.0f;
	float m_rotationChangeForce = 180.0f;

	float m_levelSpeed = 2.0f;



};