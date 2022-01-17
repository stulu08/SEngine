#pragma once
#include "App.h"

namespace Demo {
	class Player : public Behavior{
	public:
		float speed = 8.0f;
		float sprintMultiply = 2.0f;
		float sensitivity = .4f;
		virtual void onStart() override;
		virtual void onUpdate() override;
	};
}