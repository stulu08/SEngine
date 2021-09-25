#pragma once
#include "Stulu/Scene/GameObject.h"
namespace Stulu {
	class Behavior {
	public:
		template<typename T>
		T& getComponent() {
			return gameObject.getComponent<T>();
		}
	
		GameObject gameObject;
	private:
		friend class Scene;
	};
}