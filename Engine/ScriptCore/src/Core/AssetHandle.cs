using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Stulu {
	/// <summary>
	/// Refrence to an Asset
	/// </summary>
	public class AssetHandle {
		internal UInt64 assetID = UUID.Null;
		internal UInt32 gcHandle = 0;

		/// <summary>
		/// Initlize with ID
		/// </summary>
		internal AssetHandle(UUID id, bool weakRef = false) {
			if (weakRef)
			{
				InitilizeWeak(id);
			}
			else
			{
				Initilize(id);
			}
		}
		/// <summary>
		/// Maybe gets called, maybe not idk
		/// </summary>
		~AssetHandle() {
			if (gcHandle != 0)
				Release();
		}

		/// <summary>
		/// Initilizes the asset handle, creates a tracking instance to confirm ownership
		/// </summary>
		public void Initilize(ulong uuid) {
			InitilizeWeak(uuid);
			Lock();
		}
		/// <summary>
		/// Initilizes the asset handle, creates no tracking instance to confirm ownership
		/// </summary>
		public void InitilizeWeak(ulong uuid)
		{
			if (this.gcHandle != 0)
			{
				Log.EngineWarn("Asset already asigned, old refrence will be released!");
				Release();
			}
			if (uuid == UUID.Null)
			{
				Log.EngineWarn("Using invalid handle '0' for asset!");
			}
			this.assetID = uuid;
			this.gcHandle = 0;
		}

		/// <summary>
		/// Locks to asset, basicly tells the internals that this object is owned by someone
		/// </summary>
		public void Lock()
		{
			this.gcHandle = InitAssetHandle(this, this.assetID);
		}
		/// <summary>
		/// Releases the tracking instance, asset will be deleted if not other refrences exists
		/// </summary>
		public void Release()
		{
			DropAssetHandle(gcHandle, this.assetID);
			gcHandle = 0;
		}

		/// <summary>
		/// The actual id
		/// </summary>
		public UUID ID => assetID;
		/// <summary>
		/// Returns true if the asset is loaded and valid
		/// </summary>
		public bool Loaded => GetAssetLoaded(assetID);
		/// <summary>
		/// Returns true if the asset is loaded and valid
		/// </summary>
		public bool Valid => assetID != UUID.Null;
		/// <summary>
		/// Returns the path for the asset, ":" or empty if loaded from memory
		/// </summary>
		public string Path => GetAssetPath(assetID);
		/// <summary>
		/// See path for details
		/// </summary>
		public bool IsMemoryLoaded => CheckMemoryPath();

		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static void DropAssetHandle(UInt32 gcHandle, ulong assetID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static UInt32 InitAssetHandle(object obj, ulong assetID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static bool GetAssetLoaded(ulong assetID);
		[MethodImpl(MethodImplOptions.InternalCall)]
		internal extern static string GetAssetPath(ulong assetID);

		internal bool CheckMemoryPath()
		{
			string path = GetAssetPath(assetID);
			return path.Length == 0 && path.Contains(":");
		}

		/// <summary>
		/// Static method to get the asset id, works even if object is null
		/// </summary>
		public static UUID GetIDSafe<T>(T assetObject) where T : AssetHandle
		{
			if(assetObject == null) return UUID.Null;
			return assetObject.ID;
		}
	}
}
