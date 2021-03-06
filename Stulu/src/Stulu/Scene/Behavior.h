#pragma once
#include "Stulu/Scene/GameObject.h"
namespace Stulu {
	class STULU_API Behavior {
	public:
		virtual ~Behavior() { };

		template<typename T>
		T& getComponent() {
			return gameObject.getComponent<T>();
		}
	protected:
		GameObject gameObject;

		/// <summary>
		/// On Scene initilization
		/// </summary>
		virtual void onStart() { }
		/// <summary>
		/// On Scribt initilization
		/// </summary>
		virtual void onAwake() { }
		/// <summary>
		/// On Update 
		/// </summary>
		virtual void onUpdate() { }
		/// <summary>
		/// After every GameObject is drawn
		/// </summary>
		/// <param name="timestep">Last Frametime without scaling</param>
		virtual void onRender(Timestep timestep) { }
		/// <summary>
		/// When the Component is destroyed
		/// </summary>
		virtual void onDestroy() { }
		/// <summary>
		/// When the scene exits
		/// </summary>
		virtual void onSceneExit() {}
	
	private:
		friend class Scene;
	};
}