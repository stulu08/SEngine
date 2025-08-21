using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Stulu
{
	/// <summary>
	/// This component in internaly used to store every user c# script
	/// </summary>
	public class ScriptingComponent : GameObjectAttached {
		/// <summary>
		/// This will create an Array of the attached scripts for this gameobject
		/// </summary>
		public Component[] FetchScripts()
		{
			return InternalCalls.scriptingComp_fetchScripts(id);
		}
	}
}
