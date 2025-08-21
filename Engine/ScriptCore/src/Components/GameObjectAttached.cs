using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

#pragma warning disable 1591
namespace Stulu {
	//default components
	public class GameObjectAttached {
		public GameObject gameObject { get; internal set; }
		internal void Initilize(ulong goId) {
			gameObject = GameObject.CreateInternal(goId);
		}
		internal void InitilizeFromObject(GameObject gameObject)
		{
			ulong id = gameObject == null ? GameObject.NULL_ID : gameObject.ID;
			Initilize(id);
		}
		public ulong id { get => gameObject.ID; }

		public TransformComponent transform => gameObject.transform;
		public string name { get => this.gameObject.Name; set => this.gameObject.Name = value; }
		public string tag { get => this.gameObject.Tag; set => this.gameObject.Tag = value; }
	}
}
