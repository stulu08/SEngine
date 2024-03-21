#pragma once
#include "Stulu/Scene/GameObject.h"
namespace Stulu {
	class STULU_API NativeScript {
	public:
		virtual ~NativeScript() { };

		template<typename T>
		inline T& getComponent() {
			return gameObject.getComponent<T>();
		}
		template<typename T>
		inline T& addComponent() {
			return gameObject.addComponent<T>();
		}
		template<typename T>
		inline bool hasComponent() {
			return gameObject.hasComponent<T>();
		}

		/// <summary>
		/// This is called when the component is drawn in the inspector
		/// </summary>
		virtual void uiFunc() {}
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
		/// Before every 3D GameObject is drawn
		/// </summary>
		virtual void onRender() { }
		/// <summary>
		/// Before every 2D GameObject is drawn
		/// </summary>
		virtual void onRender2D() { }
		/// <summary>
		/// On Gizmos draw
		/// </summary>
		virtual void onDrawGizmos() { }
		/// <summary>
		/// When the Component is destroyed
		/// </summary>
		virtual void onDestroy() { }
		/// <summary>
		/// When the scene exits
		/// </summary>
		virtual void onSceneExit() {}
	private:
		friend class EventCaller;
	};
}