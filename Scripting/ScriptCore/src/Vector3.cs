using System;

namespace Stulu {
	public struct Vector3 {
        public static Vector3 Zero = new Vector3(0.0f, 0.0f, 0.0f);
        public static Vector3 One = new Vector3(1f, 1f, 1f);
        public static Vector3 Forward = new Vector3(0.0f, 0.0f, -1f);
        public static Vector3 Back = new Vector3(0.0f, 0.0f, 1f);
        public static Vector3 Right = new Vector3(1f, 0.0f, 0.0f);
        public static Vector3 Left = new Vector3(-1f, 0.0f, 0.0f);
        public static Vector3 Up = new Vector3(0.0f, 1f, 0.0f);
        public static Vector3 Down = new Vector3(0.0f, -1f, 0.0f);
        public float X;
        public float Y;
        public float Z;
        public const float kEpsilonNormalSqrt = 1E-15f;

        public Vector3(float scalar) => this.X = this.Y = this.Z = scalar;

        public Vector3(float x, float y, float z) {
            this.X = x;
            this.Y = y;
            this.Z = z;
        }

        public Vector3(float x, Vector2 yz) {
            this.X = x;
            this.Y = yz.X;
            this.Z = yz.Y;
        }

        public Vector3(Vector2 vector) {
            this.X = vector.X;
            this.Y = vector.Y;
            this.Z = 0.0f;
        }

        public Vector3(Vector4 vector) {
            this.X = vector.X;
            this.Y = vector.Y;
            this.Z = vector.Z;
        }

        public void Clamp(Vector3 min, Vector3 max) {
            this.X = Mathf.Clamp(this.X, min.X, max.X);
            this.Y = Mathf.Clamp(this.Y, min.Y, max.Y);
            this.Z = Mathf.Clamp(this.Z, min.Z, max.Z);
        }

        public float Length() => (float)Math.Sqrt((double)this.X * (double)this.X + (double)this.Y * (double)this.Y + (double)this.Z * (double)this.Z);

        public Vector3 Normalized() {
            float num = this.Length();
            float x = this.X;
            float y = this.Y;
            float z = this.Z;
            if ((double)num > 0.0) {
                x /= num;
                y /= num;
                z /= num;
            }
            return new Vector3(x, y, z);
        }

        public void Normalize() {
            float num = this.Length();
            if ((double)num <= 0.0)
                return;
            this.X /= num;
            this.Y /= num;
            this.Z /= num;
        }

        public static Vector3 DirectionFromEuler(Vector3 rotation) {
            float x = rotation.X;
            float y = rotation.Y;
            float z = rotation.Z;
            return new Vector3(Mathf.Cos(y) * Mathf.Cos(x), Mathf.Sin(y) * Mathf.Cos(x), Mathf.Sin(x));
        }

        public float Distance(Vector3 other) => (float)Math.Sqrt(Math.Pow((double)other.X - (double)this.X, 2.0) + Math.Pow((double)other.Y - (double)this.Y, 2.0) + Math.Pow((double)other.Z - (double)this.Z, 2.0));

        public static float Distance(Vector3 p1, Vector3 p2) => (float)Math.Sqrt(Math.Pow((double)p2.X - (double)p1.X, 2.0) + Math.Pow((double)p2.Y - (double)p1.Y, 2.0) + Math.Pow((double)p2.Z - (double)p1.Z, 2.0));

        public static Vector3 Lerp(Vector3 p1, Vector3 p2, float maxDistanceDelta) {
            if ((double)maxDistanceDelta < 0.0)
                return p1;
            return (double)maxDistanceDelta > 1.0 ? p2 : p1 + (p2 - p1) * maxDistanceDelta;
        }

        public float sqrMagnitude => (float)((double)this.X * (double)this.X + (double)this.Y * (double)this.Y + (double)this.Z * (double)this.Z);

        public static float Dot(Vector3 lhs, Vector3 rhs) => (float)((double)lhs.X * (double)rhs.X + (double)lhs.Y * (double)rhs.Y + (double)lhs.Z * (double)rhs.Z);

        public static float Angle(Vector3 from, Vector3 to) {
            float num = (float)Math.Sqrt((double)from.sqrMagnitude * (double)to.sqrMagnitude);
            return (double)num < 1.00000000362749E-15 ? 0.0f : (float)Math.Acos((double)Mathf.Clamp(Vector3.Dot(from, to) / num, -1f, 1f)) * 57.29578f;
        }

        public static Vector3 operator *(Vector3 left, float scalar) => new Vector3(left.X * scalar, left.Y * scalar, left.Z * scalar);

        public static Vector3 operator *(float scalar, Vector3 right) => new Vector3(scalar * right.X, scalar * right.Y, scalar * right.Z);

        public static Vector3 operator *(Vector3 left, Vector3 right) => new Vector3(left.X * right.X, left.Y * right.Y, left.Z * right.Z);

        public static Vector3 operator /(Vector3 left, Vector3 right) => new Vector3(left.X / right.X, left.Y / right.Y, left.Z / right.Z);

        public static Vector3 operator /(Vector3 left, float scalar) => new Vector3(left.X / scalar, left.Y / scalar, left.Z / scalar);

        public static Vector3 operator /(float scalar, Vector3 right) => new Vector3(scalar / right.X, scalar / right.Y, scalar / right.Z);

        public static Vector3 operator +(Vector3 left, Vector3 right) => new Vector3(left.X + right.X, left.Y + right.Y, left.Z + right.Z);

        public static Vector3 operator +(Vector3 left, float right) => new Vector3(left.X + right, left.Y + right, left.Z + right);

        public static Vector3 operator -(Vector3 left, Vector3 right) => new Vector3(left.X - right.X, left.Y - right.Y, left.Z - right.Z);

        public static Vector3 operator -(Vector3 left, float right) => new Vector3(left.X - right, left.Y - right, left.Z - right);

        public static Vector3 operator -(Vector3 vector) => new Vector3(-vector.X, -vector.Y, -vector.Z);

        public static Vector3 Cross(Vector3 x, Vector3 y) => new Vector3((float)((double)x.Y * (double)y.Z - (double)y.Y * (double)x.Z), (float)((double)x.Z * (double)y.X - (double)y.Z * (double)x.X), (float)((double)x.X * (double)y.Y - (double)y.X * (double)x.Y));

        public override bool Equals(object obj) => obj is Vector3 right && this.Equals(right);

        public bool Equals(Vector3 right) => (double)this.X == (double)right.X && (double)this.Y == (double)right.Y && (double)this.Z == (double)right.Z;

        public override int GetHashCode() => (this.X, this.Y, this.Z).GetHashCode();

        public static bool operator ==(Vector3 left, Vector3 right) => left.Equals(right);

        public static bool operator !=(Vector3 left, Vector3 right) => !(left == right);

        public static Vector3 Cos(Vector3 vector) => new Vector3((float)Math.Cos((double)vector.X), (float)Math.Cos((double)vector.Y), (float)Math.Cos((double)vector.Z));

        public static Vector3 Sin(Vector3 vector) => new Vector3((float)Math.Sin((double)vector.X), (float)Math.Sin((double)vector.Y), (float)Math.Sin((double)vector.Z));

        public override string ToString() => string.Format("Vector3[{0}, {1}, {2}]", (object)this.X, (object)this.Y, (object)this.Z);

        public float x { get => this.X; set { this.X = value; } }
        public float y { get => this.Y; set { this.Y = value; } }
        public float z { get => this.Z; set { this.Z = value; } }

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
    }
}
