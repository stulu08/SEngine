#pragma once
#include "Stulu/Scene/GameObject.h"
namespace Stulu {
	class STULU_API Component {
	public:
		//this GameObject
		GameObject gameObject = GameObject::null;

		virtual void destroy() {};
		virtual void onComponentAdded(Scene* scene) {};
	};
}