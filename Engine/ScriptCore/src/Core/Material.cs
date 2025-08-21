#pragma warning disable 1591
using System;
using System.Runtime.CompilerServices;

namespace Stulu
{
	public struct MaterialColor
	{
		public float R, G, B, A;

		public MaterialColor(float r, float  g, float b, float a = 1.0f)
		{
			this.R = r; 
			this.G = g;
			this.B = b;
			this.A = a;
		}

		public MaterialColor(Vector4 vec)
		{
			this.R = vec.r;
			this.G = vec.g;
			this.B = vec.b;
			this.A = vec.a;
		}

		public static implicit operator Vector4(MaterialColor c) => new Vector4(c.R, c.G, c.B, c.A);
	}
	public enum MaterialPropertyType
	{
		Float, Float2, Float3, Float4,
		Int, Int2, Int3, Int4,
		UInt,
		Color,
		Sampler2D,
		SkyBox,
		Cubemap,
		None
	}
	/// <summary>
	/// Refrence to a material
	/// </summary>
	public class Material : AssetHandle
	{
		private Material(UUID id, bool asWeakRef = false) : base(id, asWeakRef) { }
		internal static Material Create(UUID id)
		{
			if (id == 0)
				return null;
			return new Material(id);
		}
		internal static Material CreateWeak(UUID id)
		{
			if (id == 0)
				return null;
			return new Material(id, true);
		}

		/// <summary>
		/// Name of the material
		/// </summary>
		public string Name => GetNameInternal(ID);

		public int PropertyCount => GetPropertyCountIntenal(ID);
		public MaterialPropertyType GetPropertyType(int index) => (MaterialPropertyType)GetPropertyTypeIntenal(ID, index);
		public string GetPropertyName(int index) => GetPropertyNameIntenal(ID, index);

		public T GetProperty<T>(string name)
		{
			object result = GetPropertyInternal(ID, name, typeof(T));
			return (T)result;
		}

		public void SetProperty<T>(string name, T value)
		{
			SetPropertyInternal(ID, name, typeof(T), value);
		}

		public bool HasProperty(string name) => HasPropertyInternal(ID, name);

		private object GetPropertyInternal(ulong id, string name, Type type)
		{
			if (type == typeof(float)) return GetPropertyValueFloat(id, name);
			if (type == typeof(int)) return GetPropertyValueInt(id, name);
			if (type == typeof(uint)) return GetPropertyValueUInt(id, name);
			if (type == typeof(Vector2)) return GetPropertyValueFloat2(id, name);
			if (type == typeof(Vector3)) return GetPropertyValueFloat3(id, name);
			if (type == typeof(Vector4)) return GetPropertyValueFloat4(id, name);
			if (type == typeof(MaterialColor)) return GetPropertyValueColor(id, name);
			if (type == typeof(Texture2D)) return Texture2D.CreateWeak(GetPropertyValueSampler2D(id, name));
			throw new ArgumentException($"Unsupported type {type}");
		}

		private void SetPropertyInternal(ulong id, string name, Type type, object val)
		{
			if (type == typeof(float)) { float v = (float)val; SetPropertyValueFloat(id, name, ref v); return; }
			if (type == typeof(int)) { int v = (int)val; SetPropertyValueInt(id, name, ref v); return; }
			if (type == typeof(uint)) { uint v = (uint)val; SetPropertyValueUInt(id, name, ref v); return; }
			if (type == typeof(Vector2)) { Vector2 v = (Vector2)val; SetPropertyValueFloat2(id, name, ref v); return; }
			if (type == typeof(Vector3)) { Vector3 v = (Vector3)val; SetPropertyValueFloat3(id, name, ref v); return; }
			if (type == typeof(Vector4)) { Vector4 v = (Vector4)val; SetPropertyValueFloat4(id, name, ref v); return; }
			if (type == typeof(MaterialColor)) { MaterialColor v = (MaterialColor)val; SetPropertyValueColor(id, name, ref v); return; }
			if (type == typeof(Texture2D)) { UUID v = AssetHandle.GetIDSafe((Texture2D)val); SetPropertyValueSampler2D(id, name, ref v.ID); return; }
			throw new ArgumentException($"Unsupported type {type}");
		}

		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static bool HasPropertyInternal(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string GetNameInternal(ulong id);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static int GetPropertyCountIntenal(ulong id);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static int GetPropertyTypeIntenal(ulong id, int index);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string GetPropertyNameIntenal(ulong id, int index);


		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static float GetPropertyValueFloat(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static void SetPropertyValueFloat(ulong id, string name, ref float val);

		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static int GetPropertyValueInt(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static void SetPropertyValueInt(ulong id, string name, ref int val);

		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static uint GetPropertyValueUInt(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static void SetPropertyValueUInt(ulong id, string name, ref uint val);

		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static Vector2 GetPropertyValueFloat2(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static void SetPropertyValueFloat2(ulong id, string name, ref Vector2 val);

		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static Vector3 GetPropertyValueFloat3(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static void SetPropertyValueFloat3(ulong id, string name, ref Vector3 val);

		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static Vector4 GetPropertyValueFloat4(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static void SetPropertyValueFloat4(ulong id, string name, ref Vector4 val);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static MaterialColor GetPropertyValueColor(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void SetPropertyValueColor(ulong id, string name, ref MaterialColor val);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static ulong GetPropertyValueSampler2D(ulong id, string name);
		[MethodImpl(MethodImplOptions.InternalCall)] 
		internal extern static void SetPropertyValueSampler2D(ulong id, string name, ref ulong val);
	}
}
