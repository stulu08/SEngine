using System;
using System.Buffers;
using System.Diagnostics.PerformanceData;
using System.Reflection.Emit;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace Stulu
{
	/// <summary>
	/// Represents a native allocated array from unmanaged code
	/// </summary>
	/// <typeparam name="T"></typeparam>
	[StructLayout(LayoutKind.Sequential, Size = 16, Pack = 8)]
	public unsafe struct NativeArray<T> where T : unmanaged
	{
		/// <summary>
		/// Empty value placehodler
		/// </summary>
		public static NativeArray<T> Empty = new NativeArray<T>(0);

		/// <summary>
		/// Memory address
		/// </summary>
		[MarshalAs(UnmanagedType.SysUInt)]
		private IntPtr Pointer;
		
		[MarshalAs(UnmanagedType.SysUInt)]
		private UInt64 ArrayLength;

		/// <summary>
		/// Length of the array in elements
		/// </summary>
		public int Length => (int)ArrayLength;

		/// <summary>
		/// Default constructor, placeholder
		/// </summary>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public NativeArray(uint zeroValue = 0)
		{
			Pointer = IntPtr.Zero;
			ArrayLength = zeroValue;
		}

		/// <summary>
		/// Constructor
		/// </summary>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public NativeArray(IntPtr ptr, ulong length)
		{
			Pointer = ptr;
			ArrayLength = length;
		}

		/// <summary>
		/// Create a NativeArray from a managed array, data will be copied to a new buffer. If length is less then 0, data.Length will be used
		/// </summary>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public unsafe static NativeArray<T> CopyFromArray(T[] array, int start = 0, int length = -1)
		{
			int count = length < 0 ? array.Length : length;

			if (array == null) {
				if (start != 0 || count != 0)
					throw new ArgumentNullException("array");
				return NativeArray<T>.Empty;
			}
			if (start > array.Length || count > (array.Length - start))
				throw new ArgumentOutOfRangeException("start | count");
			if (array.GetType() != typeof(T[]))
				throw new ArrayTypeMismatchException("array");


			int SizeBytes = sizeof(T) * length;

			IntPtr ptr = Marshal.AllocHGlobal(SizeBytes);
			fixed (T* src = array)
			{
				Buffer.MemoryCopy(src, (void*)ptr, SizeBytes, SizeBytes);
			}

			return new NativeArray<T>(ptr, (ulong)length);
		}

		/// <summary>
		/// Create a NativeArray from a managed array. If length is less then 0, data.Length will be used
		/// </summary>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public static NativeArray<T> FromArray(T[] array, int start = 0, int length = -1)  
		{
			int count = length < 0 ? array.Length : length;
			Span<T> span = new Span<T>(array, start, count);
			fixed (T* src = &span.GetPinnableReference())
			{
				return new NativeArray<T>((IntPtr)src, (ulong)span.Length);
			}
		}

		/// <summary>
		/// Allocate memory, needs to be free'd
		/// </summary>
		public static NativeArray<T> Allocate(int length)
		{
			int sizeInBytes = sizeof(T) * length;
			IntPtr ptr = Marshal.AllocHGlobal(sizeInBytes);
			return new NativeArray<T>(ptr, (ulong)length);
		}

		/// <summary>
		/// Raw access to the array
		/// </summary>
		/// <param name="index"></param>
		public ref T this[int index]
		{
			[MethodImpl(MethodImplOptions.AggressiveInlining)]
			get
			{
				if ((uint)index >= (uint)Length)
					throw new IndexOutOfRangeException();

				return ref ((T*)Pointer)[index];
			}
		}

		/// <summary>
		/// Free the allocated memory, use only if manully allocated using Allocate
		/// </summary>
		[MethodImpl(MethodImplOptions.AggressiveInlining)]
		public void Free()
		{
			Marshal.FreeHGlobal(Pointer);
			Pointer = IntPtr.Zero;
			ArrayLength = 0;
		}

		/// <summary>
		/// Creates a span
		/// </summary>
		public Span<T> ToSpan()
		{
			return new Span<T>((void*)Pointer, Length);
		}

		/// <summary>
		/// Creates a span
		/// </summary>
		public Span<T> ToArray()
		{
			return new Span<T>((void*)Pointer, Length).ToArray();
		}

	}
}
