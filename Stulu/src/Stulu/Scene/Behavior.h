#pragma once
#include "Stulu/Scene/GameObject.h"
namespace Stulu {
	class Behavior {
	public:
		virtual ~Behavior() { };

		template<typename T>
		T& getComponent() {
			return gameObject.getComponent<T>();
		}
	protected:
		GameObject gameObject;

		virtual void start() { }
		virtual void update(Timestep timestep) { }
		virtual void destroy() { }
	
	private:
		friend class Scene;
	};
}