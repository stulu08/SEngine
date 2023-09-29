using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	//default components
	public class GameObjectAttached {
		public GameObject gameObject { get; internal set; }
		internal void initilize(uint goId) {
			this.gameObject = new GameObject(goId);
		}
		public uint id { get => gameObject.ID; }

		public TransformComponent transform => gameObject.transform;
		public string name { get => this.gameObject.Name; set => this.gameObject.Name = value; }
		public string tag { get => this.gameObject.Tag; set => this.gameObject.Tag = value; }
	}
}
