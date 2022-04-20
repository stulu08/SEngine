using System;

namespace Stulu {
	public struct Vector2 {
        public static Vector2 Zero = new Vector2(0.0f, 0.0f);
        public static Vector2 Right = new Vector2(1f, 0.0f);
        public static Vector2 Left = new Vector2(-1f, 0.0f);
        public static Vector2 Up = new Vector2(0.0f, 1f);
        public static Vector2 Down = new Vector2(0.0f, -1f);
        public float X;
        public float Y;

        public Vector2(float scalar) => this.X = this.Y = scalar;

        public Vector2(float x, float y) {
            this.X = x;
            this.Y = y;
        }

        public Vector2(Vector3 vector) {
            this.X = vector.X;
            this.Y = vector.Y;
        }

        public void Clamp(Vector2 min, Vector2 max) {
            this.X = Mathf.Clamp(this.X, min.X, max.X);
            this.Y = Mathf.Clamp(this.Y, min.Y, max.Y);
        }

        public float Length() => (float)Math.Sqrt((double)this.X * (double)this.X + (double)this.Y * (double)this.Y);

        public Vector2 Normalized() {
            float num = this.Length();
            float x = this.X;
            float y = this.Y;
            if ((double)num > 0.0) {
                x /= num;
                y /= num;
            }
            return new Vector2(x, y);
        }

        public void Normalize() {
            float num = this.Length();
            if ((double)num <= 0.0)
                return;
            this.X /= num;
            this.Y /= num;
        }

        public float Distance(Vector3 other) => (float)Math.Sqrt(Math.Pow((double)other.X - (double)this.X, 2.0) + Math.Pow((double)other.Y - (double)this.Y, 2.0));

        public static float Distance(Vector3 p1, Vector3 p2) => (float)Math.Sqrt(Math.Pow((double)p2.X - (double)p1.X, 2.0) + Math.Pow((double)p2.Y - (double)p1.Y, 2.0));

        public static Vector2 Lerp(Vector2 p1, Vector2 p2, float maxDistanceDelta) {
            if ((double)maxDistanceDelta < 0.0)
                return p1;
            return (double)maxDistanceDelta > 1.0 ? p2 : p1 + (p2 - p1) * maxDistanceDelta;
        }

        public static Vector2 operator *(Vector2 left, float scalar) => new Vector2(left.X * scalar, left.Y * scalar);

        public static Vector2 operator *(float scalar, Vector2 right) => new Vector2(scalar * right.X, scalar * right.Y);

        public static Vector2 operator *(Vector2 left, Vector2 right) => new Vector2(left.X * right.X, left.Y * right.Y);

        public static Vector2 operator /(Vector2 left, Vector2 right) => new Vector2(left.X / right.X, left.Y / right.Y);

        public static Vector2 operator /(Vector2 left, float scalar) => new Vector2(left.X / scalar, left.Y / scalar);

        public static Vector2 operator /(float scalar, Vector2 right) => new Vector2(scalar / right.X, scalar / right.Y);

        public static Vector2 operator +(Vector2 left, Vector2 right) => new Vector2(left.X + right.X, left.Y + right.Y);

        public static Vector2 operator +(Vector2 left, float right) => new Vector2(left.X + right, left.Y + right);

        public static Vector2 operator -(Vector2 left, Vector2 right) => new Vector2(left.X - right.X, left.Y - right.Y);

        public static Vector2 operator -(Vector2 vector) => new Vector2(-vector.X, -vector.Y);

        public override string ToString() => "Vector2[" + this.X.ToString() + ", " + this.Y.ToString() + "]";

        public float x { get => this.X; set { this.X = value; } }
        public float y { get => this.Y; set { this.Y = value; } }
    }
}
