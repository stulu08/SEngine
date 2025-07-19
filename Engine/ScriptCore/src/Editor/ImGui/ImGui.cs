using Stulu;
using System;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace Editor {
	/// <summary>
	/// Gui interface for the Editor GUI
	/// </summary>
	public static class ImGui
	{
		/// Used for tree nodes
		[System.Flags]
		public enum TreeNodeFlags
		{
			/// None
			None = 0,
			/// Draw as selected
			Selected = 1 << 0,
			/// Draw frame with background (e.g. for CollapsingHeader)
			Framed = 1 << 1,
			/// Hit testing to allow subsequent widgets to overlap this one
			AllowOverlap = 1 << 2,
			/// Don't do a TreePush() when open (e.g. for CollapsingHeader) = no extra indent nor pushing on ID stack
			NoTreePushOnOpen = 1 << 3,
			/// Don't automatically and temporarily open node when Logging is active (by default logging will automatically open tree nodes)
			NoAutoOpenOnLog = 1 << 4,
			/// Default node to be open
			DefaultOpen = 1 << 5,
			/// Open on double-click instead of simple click (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
			OpenOnDoubleClick = 1 << 6,
			/// Open when clicking on the arrow part (default for multi-select unless any _OpenOnXXX behavior is set explicitly). Both behaviors may be combined.
			OpenOnArrow = 1 << 7,
			/// No collapsing, no arrow (use as a convenience for leaf nodes).
			Leaf = 1 << 8,
			/// Display a bullet instead of arrow. IMPORTANT: node can still be marked open/close if you don't set the _Leaf flag!
			Bullet = 1 << 9,
			/// Use FramePadding (even for an unframed text node) to vertically align text baseline to regular widget height. Equivalent to calling AlignTextToFramePadding() before the node.
			FramePadding = 1 << 10,
			/// Extend hit box to the right-most edge, even if not framed. This is not the default in order to allow adding other items on the same line without using AllowOverlap mode.
			SpanAvailWidth = 1 << 11,
			/// Extend hit box to the left-most and right-most edges (cover the indent area).
			SpanFullWidth = 1 << 12,
			/// Narrow hit box + narrow hovering highlight, will only cover the label text.
			SpanLabelWidth = 1 << 13,
			/// Frame will span all columns of its container table (label will still fit in current column)
			SpanAllColumns = 1 << 14,
			/// Label will span all columns of its container table
			LabelSpanAllColumns = 1 << 15,
			/// (WIP) Nav: left direction may move to this TreeNode() from any of its child (items submitted between TreeNode and TreePop)
			NavLeftJumpsBackHere = 1 << 17,
		}
		/// <summary>
		/// Draws text
		/// </summary>
		public static void Text(string text) => EditorCalls.ImGui_Text(text);
		/// <summary>
		/// Draws a vector 2 control
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Vector2(string name, ref Vector2 value, float speed = 1.0f) => EditorCalls.ImGui_Vector2(name, ref value, speed);
		/// <summary>
		/// Draws a vector 3 control
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Vector3(string name, ref Vector3 value, float speed = 1.0f) => EditorCalls.ImGui_Vector3(name, ref value, speed);
		/// <summary>
		/// Draws a float control
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Float(string name, ref float value, float min = System.Single.MinValue, float max = System.Single.MaxValue, float speed = 1.0f) => EditorCalls.ImGui_Float(name, ref value, min, max, speed);
		/// <summary>
		/// Draws an int control
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Int(string name, ref Int32 value, Int32 min = System.Int32.MinValue, Int32 max = System.Int32.MaxValue, float speed = 1.0f) => EditorCalls.ImGui_Int(name, ref value, min, max, speed);
		/// <summary>
		/// Draws an uint control
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool UInt(string name, ref UInt32 value, UInt32 min = System.UInt32.MinValue, UInt32 max = System.UInt32.MaxValue, float speed = 1.0f) => EditorCalls.ImGui_UInt(name, ref value, min, max, speed);
		/// <summary>
		/// Draws a checkbox
		/// </summary>
		/// <returns>If the value has changed</returns>
		public static bool Bool(string name, ref bool value) => EditorCalls.ImGui_Bool(name, ref value);
		/// <summary>
		/// Begins a Tree node
		/// </summary>
		public static bool TreeNode(string name, TreeNodeFlags flags) => EditorCalls.ImGui_TreeNode(name, (uint)flags);
		/// <summary>
		/// Call this if TreeNode(...) == true
		/// </summary>
		public static void TreePop() => EditorCalls.ImGui_TreePop();
		/// <summary>
		/// Draws a Drop Down Combo Control
		/// </summary>
		public static bool Combo(string name, ref int currentValue, string[] names)
		{
			string namesString = "";
			foreach (string n in names)
			{
				namesString += n + '@';
			}
			if (namesString.Length == 0)
				return false;

			if (namesString[namesString.Length - 1] == '@')
				namesString.Remove(namesString.Length - 1);

			return EditorCalls.ImGui_Combo(name, ref currentValue, namesString);
		}
		/// <summary>
		/// Draws a Drop Down Combo Control
		/// </summary>
		public static bool Combo<E>(string name, ref E currentValue) where E : Enum
		{
			int val = Convert.ToInt32(currentValue);
			if(Combo(name, ref val, Enum.GetNames(typeof(E))))
			{
				currentValue = (E)Enum.ToObject(typeof(E), val);
				return true;
			}
			return false;

		}
		/// <summary>
		/// Pushes an id to the id stack, use this if you have controls with same name in a context
		/// </summary>
		public static void PushID(string id) => EditorCalls.ImGui_PushID(id);
		/// <summary>
		/// Pops the last it from the stack
		/// </summary>
		public static void PopID() => EditorCalls.ImGui_PopID();
		/// <summary>
		/// Creates a button, returns true if pressed
		/// </summary>
		public static bool Button(string name) => EditorCalls.ImGui_Button(name);
		/// <summary>
		/// Draws a texture control, can change the texture to null
		/// </summary>
		public static bool Texture2D(string name, ref Texture2D value) {
			ulong assetID = 0;
			if (value != null)
			{
				assetID = value.assetID;
			}

			if (EditorCalls.ImGui_Texture2D(name, ref assetID))
			{
				value = Stulu.Texture2D.Create(assetID);
				return true;
			}
			return false;
		}
		/// <summary>
		/// Draws a texture control, can change the material to null
		/// </summary>
		public static bool Material(string name, ref Material value)
		{
			ulong assetID = 0;
			if (value != null)
			{
				assetID = value.assetID;
			}

			if (EditorCalls.ImGui_Material(name, ref assetID))
			{
				value = Stulu.Material.Create(assetID);
				return true;
			}
			return false;
		}
		/// <summary>
		/// Draws a texture control, can change the material to null
		/// </summary>
		public static bool Mesh(string name, ref Mesh value)
		{
			ulong assetID = 0;
			if (value != null)
			{
				assetID = value.assetID;
			}

			if (EditorCalls.ImGui_Mesh(name, ref assetID))
			{
				value = Stulu.Mesh.Create(assetID);
				return true;
			}
			return false;
		}
		/// <summary>
		/// Draws a gameobject control, can change the object to null
		/// </summary>
		public static bool GameObject(string name, ref GameObject value)
		{
			ulong registryID = 0;
			if (value != null)
			{
				registryID = value.ID;
			}

			if (EditorCalls.ImGui_GameObject(name, ref registryID))
			{
				value = Stulu.GameObject.CreateInternal(registryID);
				return true;
			}
			return false;
		}
		/// <summary>
		/// Gets back to previous line and continue with horizontal layout
		/// <para> 
		/// offset_from_start_x == 0 : follow right after previous item <br />
		/// offset_from_start_x != 0 : align to specified x position (relative to window/group left) 
		/// </para>
		/// <para> 
		/// spacing_w &lt; 0         : use default spacing if offset_from_start_x == 0, no spacing if offset_from_start_x != 0  <br />
		/// spacing_w >= 0           : enforce spacing amount
		/// </para>
		/// </summary>
		public static void SameLine(float offset_from_start_x = 0.0f, float spacing_w = -1.0f) => EditorCalls.ImGui_SameLine(offset_from_start_x, spacing_w);
		
		/// <summary>
		/// Move content position toward the right, by indent_w, or style.IndentSpacing if indent_w &lt;= 0
		/// </summary>
		public static void Indent(float indent_w = 0.0f) => EditorCalls.Indent(indent_w);
		/// <summary>
		/// Move content position back to the left, by indent_w, or style.IndentSpacing if indent_w &lt;= 0
		/// </summary>
		public static void Unindent(float indent_w = 0.0f) => EditorCalls.Unindent(indent_w);

	}
}
