using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
	public class GameObjectAttached {
		public GameObject gameObject { get; internal set; }
		internal void initilize(uint goId) {
			this.gameObject = new GameObject(goId);
		}
		internal uint id { get => gameObject.id; }
	}
}
