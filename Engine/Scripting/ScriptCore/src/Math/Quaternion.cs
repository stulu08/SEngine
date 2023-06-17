#pragma warning disable 1591
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Stulu {
    public struct Quaternion : IEquatable<Quaternion> {
        public static Quaternion Identy = new Quaternion(0, 0, 0, 1);
		public float W;
        public float X;
        public float Y;
        public float Z;

        public Quaternion(float x, float y, float z, float w) {
            this.X = x;
            this.Y = y;
            this.Z = z;
            this.W = w;
        }

        public Quaternion(Vector3 xyz, float w) {
            this.X = xyz.X;
            this.Y = xyz.Y;
            this.Z = xyz.Z;
            this.W = w;
        }

        public Quaternion(Vector3 euler) {
            Vector3 vector3_1 = Vector3.Cos(euler * 0.5f);
            Vector3 vector3_2 = Vector3.Sin(euler * 0.5f);
            this.W = (float)((double)vector3_1.X * (double)vector3_1.Y * (double)vector3_1.Z + (double)vector3_2.X * (double)vector3_2.Y * (double)vector3_2.Z);
            this.X = (float)((double)vector3_2.X * (double)vector3_1.Y * (double)vector3_1.Z - (double)vector3_1.X * (double)vector3_2.Y * (double)vector3_2.Z);
            this.Y = (float)((double)vector3_1.X * (double)vector3_2.Y * (double)vector3_1.Z + (double)vector3_2.X * (double)vector3_1.Y * (double)vector3_2.Z);
            this.Z = (float)((double)vector3_1.X * (double)vector3_1.Y * (double)vector3_2.Z - (double)vector3_2.X * (double)vector3_2.Y * (double)vector3_1.Z);
        }

        public static Vector3 operator *(Quaternion q, Vector3 v) {
            Vector3 x = new Vector3(q.X, q.Y, q.Z);
            Vector3 y = Vector3.Cross(x, v);
            Vector3 vector3 = Vector3.Cross(x, y);
            return v + (y * q.W + vector3) * 2f;
        }

        public Vector3 XYZ {
            get => new Vector3(this.X, this.Y, this.Z);
            set {
                this.X = value.X;
                this.Y = value.Y;
                this.Z = value.Z;
            }
        }

        public override int GetHashCode() => (this.W, this.X, this.Y, this.Z).GetHashCode();

        public override bool Equals(object obj) => obj is Quaternion right && this.Equals(right);

        public bool Equals(Quaternion right) => (double)this.W == (double)right.W && (double)this.X == (double)right.X && (double)this.Y == (double)right.Y && (double)this.Z == (double)right.Z;

        public static bool operator ==(Quaternion left, Quaternion right) => left.Equals(right);

        public static bool operator !=(Quaternion left, Quaternion right) => !(left == right);

        public float Length => (float)Math.Sqrt((double)this.LengthSquared);

        public float LengthSquared => (float)((double)this.X * (double)this.X + (double)this.Y * (double)this.Y + (double)this.Z * (double)this.Z + (double)this.W * (double)this.W);

        public static Quaternion FromToRotation(Vector3 aFrom, Vector3 aTo) {
            Vector3 vector3 = Vector3.Cross(aFrom, aTo);
            return Quaternion.AngleAxis(Vector3.Angle(aFrom, aTo), vector3.Normalized());
        }

        public static Quaternion AngleAxis(float aAngle, Vector3 aAxis) {
            aAxis.Normalize();
            float num = (float)((double)aAngle * (Math.PI / 180.0) * 0.5);
            aAxis *= Mathf.Sin(num);
            return new Quaternion(aAxis.X, aAxis.Y, aAxis.Z, Mathf.Cos(num));
        }

        public static Quaternion QuaternionLookRotation(Vector3 forward, Vector3 up) {
            forward.Normalize();
            Vector3 vector3_1 = forward.Normalized();
            Vector3 y1 = Vector3.Cross(up, vector3_1).Normalized();
            Vector3 vector3_2 = Vector3.Cross(vector3_1, y1);
            float x1 = y1.X;
            float y2 = y1.Y;
            float z1 = y1.Z;
            float x2 = vector3_2.X;
            float y3 = vector3_2.Y;
            float z2 = vector3_2.Z;
            float x3 = vector3_1.X;
            float y4 = vector3_1.Y;
            float z3 = vector3_1.Z;
            float num1 = x1 + y3 + z3;
            Quaternion quaternion = new Quaternion();
            if ((double)num1 > 0.0) {
                float num2 = (float)Math.Sqrt((double)num1 + 1.0);
                quaternion.W = num2 * 0.5f;
                float num3 = 0.5f / num2;
                quaternion.X = (z2 - y4) * num3;
                quaternion.Y = (x3 - z1) * num3;
                quaternion.Z = (y2 - x2) * num3;
                return quaternion;
            }
            if ((double)x1 >= (double)y3 && (double)x1 >= (double)z3) {
                float num4 = (float)Math.Sqrt(1.0 + (double)x1 - (double)y3 - (double)z3);
                float num5 = 0.5f / num4;
                quaternion.X = 0.5f * num4;
                quaternion.Y = (y2 + x2) * num5;
                quaternion.Z = (z1 + x3) * num5;
                quaternion.W = (z2 - y4) * num5;
                return quaternion;
            }
            if ((double)y3 > (double)z3) {
                float num6 = (float)Math.Sqrt(1.0 + (double)y3 - (double)x1 - (double)z3);
                float num7 = 0.5f / num6;
                quaternion.X = (x2 + y2) * num7;
                quaternion.Y = 0.5f * num6;
                quaternion.Z = (y4 + z2) * num7;
                quaternion.W = (x3 - z1) * num7;
                return quaternion;
            }
            float num8 = (float)Math.Sqrt(1.0 + (double)z3 - (double)x1 - (double)y3);
            float num9 = 0.5f / num8;
            quaternion.X = (x3 + z1) * num9;
            quaternion.Y = (y4 + z2) * num9;
            quaternion.Z = 0.5f * num8;
            quaternion.W = (y2 - x2) * num9;
            return quaternion;
        }

        public void Normalize() {
            float num = 1f / this.Length;
            this.XYZ *= num;
            this.W *= num;
        }

        public static Quaternion Slerp(Quaternion a, Quaternion b, float t) {
            if ((double)t > 1.0)
                t = 1f;
            if ((double)t < 0.0)
                t = 0.0f;
            return Quaternion.SlerpUnclamped(a, b, t);
        }

        public static Quaternion SlerpUnclamped(Quaternion a, Quaternion b, float t) {
            if ((double)a.LengthSquared == 0.0)
                return (double)b.LengthSquared == 0.0 ? new Quaternion(0.0f, 0.0f, 0.0f, 1f) : b;
            if ((double)b.LengthSquared == 0.0)
                return a;
            float d = a.W * b.W + Vector3.Dot(a.XYZ, b.XYZ);
            if ((double)d >= 1.0 || (double)d <= -1.0)
                return a;
            if ((double)d < 0.0) {
                b.XYZ = -b.XYZ;
                b.W = -b.W;
                d = -d;
            }
            float num1;
            float num2;
            if ((double)d < 0.990000009536743) {
                float a1 = (float)Math.Acos((double)d);
                float num3 = 1f / (float)Math.Sin((double)a1);
                num1 = (float)Math.Sin((double)a1 * (1.0 - (double)t)) * num3;
                num2 = (float)Math.Sin((double)a1 * (double)t) * num3;
            } else {
                num1 = 1f - t;
                num2 = t;
            }
            Quaternion quaternion = new Quaternion(num1 * a.XYZ + num2 * b.XYZ, (float)((double)num1 * (double)a.W + (double)num2 * (double)b.W));
            if ((double)quaternion.LengthSquared <= 0.0)
                return new Quaternion(0.0f, 0.0f, 0.0f, 1f);
            quaternion.Normalize();
            return quaternion;
        }
        public override string ToString() => "Quaternion[" + this.X.ToString() + ", " + this.Y.ToString() + ", " + this.Z.ToString() + ", " + this.W.ToString() + "]";

        public float x { get => this.X; set { this.X = value; } }
        public float y { get => this.Y; set { this.Y = value; } }
        public float z { get => this.Z; set { this.Z = value; } }
        public float w { get => this.W; set { this.W = value; } }
        public float Pitch { get => this.X; set { this.X = value; } }
        public float Yaw { get => this.Y; set { this.Y = value; } }
        public float Roll { get => this.Z; set { this.Z = value; } }
        public float pitch { get => this.X; set { this.X = value; } }
        public float yaw { get => this.Y; set { this.Y = value; } }
        public float roll { get => this.Z; set { this.Z = value; } }

        public Vector3 xyz {
            get => new Vector3(this.X, this.Y, this.X);
            set {
                this.X = value.X;
                this.Y = value.Y;
                this.Z = value.Z;
            }
        }
    }
}
