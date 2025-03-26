#pragma warning disable 1591
using System;

namespace Editor
{
	/// <summary>
	/// Use this for creting your own Inspectors
	/// </summary>
	[AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = false)]
	public class InspectorRendererAttribute : Attribute
	{
		public InspectorRendererAttribute(Type type)
		{
			this.type = type;
			this.name = type.Name;
			this.priority = 5000;
		}
		public InspectorRendererAttribute(Type type, string name, int priority = 5000)
		{
			this.type = type;
			this.name = name;
			this.priority = priority;
		}
		public Type type;
		public string name;
		public int priority;
	}
}
