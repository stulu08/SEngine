using Stulu;
using System;
using System.Reflection;

namespace Editor
{
	/// <summary>
	/// API for interacting with 
	/// </summary>
	public static class Editor {
		/// <summary>
		/// Returns true if the editor is currently in rumtime mode (The play button is pressed)
		/// </summary>
		public static bool IsRuntime() => EditorCalls.Editor_IsRuntime();

		static Vector3 value = Vector3.Zero;
		static Texture2D texture = null;
		static uint assignTextureID = 304;
		static void DebugTest()
		{
			if (!IsRuntime())
			{
				if (ImGui.Vector3("Test Value", ref value, 0.5f))
					Log.Info("Value Channged");

				if(ImGui.TreeNode("Asset Handle Test", ImGui.TreeNodeFlags.None))
				{
					ImGui.Texture2D("Texture", ref texture);
					if(texture != null)
					{
						ImGui.Text($"Texture: {texture.ID}");

						ImGui.UInt("New ID", ref assignTextureID);
						ImGui.SameLine();
						if (ImGui.Button("Assign"))
						{
							texture = Texture2D.Create((UUID)assignTextureID);
						}

						if (ImGui.Button("Release"))
						{
							texture = null;
						}
					}
					ImGui.TreePop();
				}

			}
		}
		/// <summary>
		/// Check if there is an InspectorRenderer implemented for this type
		/// </summary>
		public static bool HasInspector(Type type) => EditorCalls.Editor_TypeHasInspector(type);

	}
}
