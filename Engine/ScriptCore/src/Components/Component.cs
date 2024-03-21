using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	///scripting components
	[System.Serializable]
	public abstract class Component : GameObjectAttached {
		/// <summary>
		/// On Script initilization
		/// </summary>
		public virtual void onAwake() { }
		/// <summary>
		/// On Scene initilization
		/// </summary>
		public virtual void onStart() { }
		/// <summary>
		/// On Update 
		/// </summary>
		public virtual void onUpdate() { }
		/// <summary>
		/// Befor every 3D Object is drawn
		/// </summary>
		public virtual void onRender() { }
		/// <summary>
		/// Before every 2D Object is drawn
		/// </summary>
		public virtual void onRender2D() { }
		/// <summary>
		/// On Gizmos Draw
		/// </summary>
		public virtual void onDrawGizmos() { }
		/// <summary>
		/// When the Component is destroyed
		/// </summary>
		public virtual void onDestroy() { }
		/// <summary>
		/// When the scene exits
		/// </summary>
		public virtual void onSceneExit() { }

		internal void Impl_onAwake() {
			onAwake();
		}
		internal void Impl_onStart() {
			onStart();
		}
		internal void Impl_onUpdate() {
			onUpdate();
		}
		internal void Impl_onRender() {
			onRender();
		}
		internal void Impl_onRender2D() {
			onRender2D();
		}
		internal void Impl_onDrawGizmos() {
			onDrawGizmos();
		}
		internal void Impl_onDestroy() {
			onDestroy();
		}
		internal void Impl_onSceneExit() {
			onSceneExit();
		}
	}
}
