using System;
#pragma warning disable 1591

namespace Stulu {
	[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field, AllowMultiple = false, Inherited = false)]
	public class ShowInEditorAttribute : Attribute {
		public bool isReadonly = true;
	}
}
