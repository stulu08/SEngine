using System;
// Decompiled with JetBrains decompiler
// Type: Hazel.Vector4
// Assembly: Hazel-ScriptCore, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null
// MVID: C14E7322-EDE5-4F3C-9776-FE9C2723B8FF

namespace Stulu {
	public struct Vector4 {
        public float X;
        public float Y;
        public float Z;
        public float W;

        public Vector4(float scalar) => this.X = this.Y = this.Z = this.W = scalar;

        public Vector4(float x, float y, float z, float w) {
            this.X = x;
            this.Y = y;
            this.Z = z;
            this.W = w;
        }

        public static Vector4 operator +(Vector4 left, Vector4 right) => new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);

        public static Vector4 operator -(Vector4 left, Vector4 right) => new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);

        public static Vector4 operator *(Vector4 left, Vector4 right) => new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);

        public static Vector4 operator *(Vector4 left, float scalar) => new Vector4(left.X * scalar, left.Y * scalar, left.Z * scalar, left.W * scalar);

        public static Vector4 operator *(float scalar, Vector4 right) => new Vector4(scalar * right.X, scalar * right.Y, scalar * right.Z, scalar * right.W);

        public static Vector4 operator /(Vector4 left, Vector4 right) => new Vector4(left.X / right.X, left.Y / right.Y, left.Z / right.Z, left.W / right.W);

        public static Vector4 operator /(Vector4 left, float scalar) => new Vector4(left.X / scalar, left.Y / scalar, left.Z / scalar, left.W / scalar);

        public static Vector4 Lerp(Vector4 a, Vector4 b, float t) {
            if ((double)t < 0.0)
                t = 0.0f;
            if ((double)t > 1.0)
                t = 1f;
            return (1f - t) * a + t * b;
        }
        public override string ToString() => "Vector4[" + this.X.ToString() + ", " + this.Y.ToString() + ", " + this.Z.ToString() + ", " + this.W.ToString() + "]";

        public float x { get => this.X; set { this.X = value; } }
        public float y { get => this.Y; set { this.Y = value; } }
        public float z { get => this.Z; set { this.Z = value; } }
        public float w { get => this.W; set { this.W = value; } }
        public float a { get => this.W; set { this.W = value; } }

        public Vector3 xyz {
            get => new Vector3(this.X, this.Y, this.X);
            set {
                this.X = value.X;
                this.Y = value.Y;
                this.Z = value.Z;
            }
        }
        public Vector2 xy {
            get => new Vector2(this.X, this.Y);
            set {
                this.X = value.X;
                this.Y = value.Y;
            }
        }

        public Vector2 xz {
            get => new Vector2(this.X, this.Z);
            set {
                this.X = value.X;
                this.Z = value.Y;
            }
        }

        public Vector2 yz {
            get => new Vector2(this.Y, this.Z);
            set {
                this.Y = value.X;
                this.Z = value.Y;
            }
        }
        public Vector3 XYZ {
            get => new Vector3(this.X, this.Y, this.X);
            set {
                this.X = value.X;
                this.Y = value.Y;
                this.Z = value.Z;
            }
        }
        public Vector2 XY {
            get => new Vector2(this.X, this.Y);
            set {
                this.X = value.X;
                this.Y = value.Y;
            }
        }

        public Vector2 XZ {
            get => new Vector2(this.X, this.Z);
            set {
                this.X = value.X;
                this.Z = value.Y;
            }
        }

        public Vector2 YZ {
            get => new Vector2(this.Y, this.Z);
            set {
                this.Y = value.X;
                this.Z = value.Y;
            }
        }
    }
}
