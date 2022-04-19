using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	[System.Serializable]
	public abstract class Component : GameObjectAttached {
		/// <summary>
		/// On Scribt initilization
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
		/// After every Object is drawn
		/// </summary>
		public virtual void onRender() { }
		/// <summary>
		/// On Gui Draw
		/// </summary>
		public virtual void onDrawEditorGUI() { }
		/// <summary>
		/// When the Component is destroyed
		/// </summary>
		public virtual void onDestroy() { }
		/// <summary>
		/// When the scene exits
		/// </summary>
		public virtual void onSceneExit() { }

		public TransformComponent transform => gameObject.transform;
		public string name { get => this.gameObject.Name; set => this.gameObject.Name = value; }
		public string tag { get => this.gameObject.Tag; set => this.gameObject.Tag = value; }
	}
}
