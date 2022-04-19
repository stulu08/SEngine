using System;

namespace Stulu {
	[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field, AllowMultiple = false, Inherited = false)]
	public class ShowInEditorAttribute : Attribute {
		public bool isReadonly = true;
	}
}
