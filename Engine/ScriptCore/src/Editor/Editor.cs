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
		static Material material = null;
		static Mesh mesh = null;
		static uint assignTextureID = 304;
		static void DebugTest()
		{
			if (!IsRuntime())
			{

				if (ImGui.TreeNode("Asset Handle Test", ImGui.TreeNodeFlags.None))
				{
					ImGui.Texture2D("Texture", ref texture);
					if(texture != null)
					{
						ImGui.Text($"Texture: {texture.ID}");

						ImGui.UInt("New ID", ref assignTextureID);
						if (ImGui.Button("Assign"))
						{
							texture = Texture2D.Create((UUID)assignTextureID);
						}
						ImGui.SameLine();
						if (ImGui.Button("Release"))
						{
							texture = null;
						}

						if (ImGui.TreeNode("Settings", ImGui.TreeNodeFlags.None))
						{
							ImGui.Combo("Filtering", ref texture.Settigns.Filtering);

							ImGui.TreePop();
						}
						if (ImGui.Button("Update"))
						{
							texture.UpdateTextureSettings();
						}
					}
					ImGui.TreePop();
				}

				if(ImGui.TreeNode("Material Test", ImGui.TreeNodeFlags.None))
				{
					ImGui.Material("Material", ref material);

					if(material != null)
					{
						ImGui.Text($"Name: {material.Name}");
						int count = material.PropertyCount;
						if (ImGui.TreeNode($"Properties({count})", ImGui.TreeNodeFlags.None))
						{
							for(int i = 0; i < count; i++)
							{
								ImGui.Text($"{material.GetPropertyName(i)}: {material.GetPropertyType(i).ToString()}");
							}

							ImGui.TreePop();
						}
					}

					ImGui.TreePop();
				}

				if (ImGui.TreeNode("Mesh Test", ImGui.TreeNodeFlags.None))
				{
					ImGui.Mesh("Mesh", ref mesh);

					if (mesh != null)
					{
						ImGui.Text($"Name: {mesh.Name}");

						ImGui.Text($"Vertices: {mesh.Vertices.Length}");
						ImGui.Text($"Indices: {mesh.Indices.Length}");

						int count = mesh.SubMeshCount;
						if (ImGui.TreeNode($"Sub Meshes({count})", ImGui.TreeNodeFlags.None))
						{
							for (int i = 0; i < count; i++)
							{
								ImGui.Text(mesh.GetSubMesh(i).name);
							}

							ImGui.TreePop();
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
