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
    }
}
