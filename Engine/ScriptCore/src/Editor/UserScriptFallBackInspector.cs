using Stulu;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.Remoting.Messaging;

namespace Editor
{
	[InspectorRenderer(typeof(ScriptingComponent), "##UserScripts")]
	internal class UserScriptFallBackInspector : Inspector
	{
		private Dictionary<Type, bool> inspectorCache = new Dictionary<Type, bool>();

		public override void Render(GameObject gameObject) {
			ScriptingComponent scriptingComponent = gameObject.getComponent<ScriptingComponent>();
			if (scriptingComponent == null) return;


			Component[] scripts = scriptingComponent.FetchScripts();
			if(scripts == null || scripts.Length == 0 ) return;

			foreach (Component script in scripts) {
				if (inspectorCache.ContainsKey(script.GetType())) {
					if (inspectorCache[script.GetType()] == true)
						return;
				}
				bool hasInspector = Editor.HasInspector(script.GetType());
				inspectorCache[script.GetType()] = hasInspector;
				if (hasInspector) return;

				if(ImGui.TreeNode(script.GetType().Name, ImGui.TreeNodeFlags.DefaultOpen | ImGui.TreeNodeFlags.Framed)) {
					RenderScript(script);
					ImGui.TreePop();
				}
			}
		}

		public void RenderScript(Component component) { 
			foreach(FieldInfo field in component.GetType().GetFields())
			{
				if(field.GetCustomAttribute<ShowInEditorAttribute>() == null)
				{
					continue;
				}
				
				Type fieldType = field.FieldType;

				if (fieldType == typeof(Vector3)) {
					Vector3 value = (Vector3)field.GetValue(component);
					if (ImGui.Vector3(field.Name, ref value)) { 
						field.SetValue(component, value);
					}
					continue;
				}
				if (fieldType == typeof(bool))
				{
					bool value = (bool)field.GetValue(component);
					if (ImGui.Bool(field.Name, ref value))
					{
						field.SetValue(component, value);
					}
					continue;
				}
				if (fieldType == typeof(float))
				{
					float value = (float)field.GetValue(component);
					if (ImGui.Float(field.Name, ref value))
					{
						field.SetValue(component, value);
					}
					continue;
				}

			}
		}
	}
}
