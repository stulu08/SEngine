using System;
// Decompiled with JetBrains decompiler
// Type: Hazel.Vector4
// Assembly: Hazel-ScriptCore, Version=0.0.0.0, Culture=neutral, PublicKeyToken=null
// MVID: C14E7322-EDE5-4F3C-9776-FE9C2723B8FF

namespace Stulu {
	public struct Vector4 {
        public static Vector4 Zero = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
        public static Vector4 One = new Vector4(1f, 1f, 1f, 1f);

        public float X;
        public float Y;
        public float Z;
        public float W;

        public Vector4(float scalar) => this.X = this.Y = this.Z = this.W = scalar;

        public Vector4(float x, float y, float z, float w = 1.0f) {
            this.X = x;
            this.Y = y;
            this.Z = z;
            this.W = w;
        }
        public Vector4(Vector3 xyz, float w = 1.0f) {
            this.X = xyz.x;
            this.Y = xyz.y;
            this.Z = xyz.z;
            this.W = w;
        }

        public static Vector4 operator +(Vector4 left, Vector4 right) => new Vector4(left.X + right.X, left.Y + right.Y, left.Z + right.Z, left.W + right.W);

        public static Vector4 operator -(Vector4 left, Vector4 right) => new Vector4(left.X - right.X, left.Y - right.Y, left.Z - right.Z, left.W - right.W);

        public static Vector4 operator *(Vector4 left, Vector4 right) => new Vector4(left.X * right.X, left.Y * right.Y, left.Z * right.Z, left.W * right.W);

        public static Vector4 operator *(Vector4 left, float scalar) => new Vector4(left.X * scalar, left.Y * scalar, left.Z * scalar, left.W * scalar);

        public static Vector4 operator *(float scalar, Vector4 right) => new Vector4(scalar * right.X, scalar * right.Y, scalar * right.Z, scalar * right.W);

        public static Vector4 operator /(Vector4 left, Vector4 right) => new Vector4(left.X / right.X, left.Y / right.Y, left.Z / right.Z, left.W / right.W);

        public static Vector4 operator /(Vector4 left, float scalar) => new Vector4(left.X / scalar, left.Y / scalar, left.Z / scalar, left.W / scalar);

        public float this[int index] {
            get {
                switch (index) {
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
            set {
                switch (index) {
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
        public static Vector4 operator *(Matrix4 left, Vector4 right) => new Vector4() {
            x = left.a * right.x + left.b * right.y + left.c * right.z + left.d * right.w,
            y = left.e * right.x + left.f * right.y + left.g * right.z + left.h * right.w,
            z = left.i * right.x + left.j * right.y + left.k * right.z + left.l * right.w,
            w = left.m * right.x + left.n * right.y + left.o * right.z + left.p * right.w,
        };
        public static Vector4 operator *(Vector4 left, Matrix4 right) => new Vector4() {
            x = right.a * left.x + right.e * left.y + right.i * left.z + right.m * left.w,
            y = right.b * left.x + right.f * left.y + right.j * left.z + right.n * left.w,
            z = right.c * left.x + right.g * left.y + right.k * left.z + right.o * left.w,
            w = right.d * left.x + right.h * left.y + right.l * left.z + right.p * left.w,
        };
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
        public float r { get => this.X; set { this.X = value; } }
        public float g { get => this.Y; set { this.Y = value; } }
        public float b { get => this.Z; set { this.Z = value; } }
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
