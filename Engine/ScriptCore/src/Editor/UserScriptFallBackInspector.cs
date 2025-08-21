using Stulu;
using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.Remoting.Messaging;

namespace Editor
{
	[InspectorRenderer(typeof(ScriptingComponent), "##UserScripts", 0)]
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

				if(EditorCalls.ImGui_InspectorTreeNodeIndent(Icons.Code + " " + script.GetType().Name, out bool removeComp, Icons.Trash)) {
					RenderScript(script);
					EditorCalls.ImGui_InspectorTreePopUnindent();
				}
				if (removeComp)
				{
					gameObject.removeComponent(script.GetType());
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

				DefaultControl(component, field);
			}
		}

		public static bool DefaultControl(Component component, FieldInfo field)
		{
			Type fieldType = field.FieldType;

			if (fieldType == typeof(Vector3))
			{
				Vector3 value = (Vector3)field.GetValue(component);
				if (ImGui.Vector3(field.Name, ref value))
				{
					field.SetValue(component, value);
				}
				return true;
			}
			if (fieldType == typeof(bool))
			{
				bool value = (bool)field.GetValue(component);
				if (ImGui.Bool(field.Name, ref value))
				{
					field.SetValue(component, value);
				}
				return true;
			}
			if (fieldType == typeof(float))
			{
				float value = (float)field.GetValue(component);
				if (ImGui.Float(field.Name, ref value))
				{
					field.SetValue(component, value);
				}
				return true;
			}
			if (fieldType == typeof(Texture2D))
			{
				Texture2D value = (Texture2D)field.GetValue(component);
				if (ImGui.Texture2D(field.Name, ref value))
				{
					field.SetValue(component, value);
				}
				return true;
			}
			if (fieldType == typeof(GameObject))
			{
				GameObject value = (GameObject)field.GetValue(component);
				if (ImGui.GameObject(field.Name, ref value))
				{
					field.SetValue(component, value);
				}
				return true;

			}
			
			return false;
		}
	}
}
