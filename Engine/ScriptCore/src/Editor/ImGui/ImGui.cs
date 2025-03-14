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
	}
}
