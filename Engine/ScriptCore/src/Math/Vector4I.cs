#pragma warning disable 1591
using System;

namespace Stulu
{
	public struct Vector4I
	{
		public static Vector4I Zero = new Vector4I(0);
		public static Vector4I One = new Vector4I(1);

		public int X;
		public int Y;
		public int Z;
		public int W;

		public Vector4I(int scalar) => this.X = this.Y = this.Z = this.W = scalar;

		public Vector4I(int x, int y, int z, int w = 1)
		{
			this.X = x;
			this.Y = y;
			this.Z = z;
			this.W = w;
		}

		public static Vector4I operator +(Vector4I left, Vector4I right) => new Vector4I(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);

		public static Vector4I operator -(Vector4I left, Vector4I right) => new Vector4I(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);

		public static Vector4I operator *(Vector4I left, Vector4I right) => new Vector4I(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);
		public static Vector4I operator *(Vector4I left, int scalar) => new Vector4I(left.X * scalar, left.Y * scalar, left.Z * scalar, left.W * scalar);

		public static Vector4I operator *(int scalar, Vector4I right) => new Vector4I(scalar * right.X, scalar * right.Y, scalar * right.Z, scalar * right.W);

		public static Vector4 operator *(Vector4I left, float scalar) => new Vector4(left.X * scalar, left.Y * scalar, left.Z * scalar, left.W * scalar);

		public static Vector4 operator *(float scalar, Vector4I right) => new Vector4(scalar * right.X, scalar * right.Y, scalar * right.Z, scalar * right.W);

		public static Vector4 operator /(Vector4I left, Vector4I right) => new Vector4(left.X / right.X, left.Y / right.Y, left.Z / right.Z, left.W / right.W);

		public static Vector4 operator /(Vector4I left, float scalar) => new Vector4(left.X / scalar, left.Y / scalar, left.Z / scalar, left.W / scalar);

		public int this[int index]
		{
			get
			{
				switch (index)
				{
					case 0:
						return x;
					case 1:
						return y;
					case 2:
						return z;
					case 3:
						return w;
					default:
						throw new System.IndexOutOfRangeException("Index " + index + " is not in range of Stulu.Vector4");
				}
			}
			set
			{
				switch (index)
				{
					case 0:
						x = value;
						break;
					case 1:
						y = value;
						break;
					case 2:
						z = value;
						break;
					case 3:
						w = value;
						break;
					default:
						throw new System.IndexOutOfRangeException("Index " + index + " is not in range of Stulu.Vector4");
				}
			}
		}

		public override string ToString() => "Vector4I[" + this.X.ToString() + ", " + this.Y.ToString() + ", " + this.Z.ToString() + ", " + this.W.ToString() + "]";

		public int x { get => this.X; set { this.X = value; } }
		public int y { get => this.Y; set { this.Y = value; } }
		public int z { get => this.Z; set { this.Z = value; } }
		public int w { get => this.W; set { this.W = value; } }
		public int r { get => this.X; set { this.X = value; } }
		public int g { get => this.Y; set { this.Y = value; } }
		public int b { get => this.Z; set { this.Z = value; } }
		public int a { get => this.W; set { this.W = value; } }

		
		public static explicit operator Vector4(Vector4I vec) => vec * 1.0f;
		
	}
}
