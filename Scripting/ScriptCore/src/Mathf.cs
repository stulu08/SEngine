using System;

namespace Stulu {
	public static class Mathf {
        public const float PI = 3.141592653f;
        public const float PI_2 = 1.570796326f;
        public const float Epsilon = 1E-05f;
        public const float TwoPI = 6.283185f;
        public const float Deg2Rad = 0.01745329f;
        public const float Rad2Deg = 57.29578f;

        public static float Sin(float value) => (float)Math.Sin((double)value);

        public static float Cos(float value) => (float)Math.Cos((double)value);

        public static float Acos(float value) => (float)Math.Acos((double)value);

        public static float Clamp(float value, float min, float max) {
            if ((double)value < (double)min)
                return min;
            return (double)value > (double)max ? max : value;
        }

        public static float Asin(float x) => (float)Math.Asin((double)x);

        public static float Atan(float x) => (float)Math.Atan((double)x);

        public static float Atan2(float y, float x) => (float)Math.Atan2((double)y, (double)x);

        public static float Min(float v0, float v1) => (double)v0 >= (double)v1 ? v1 : v0;

        public static float Max(float v0, float v1) => (double)v0 <= (double)v1 ? v1 : v0;

        public static float Sqrt(float value) => (float)Math.Sqrt((double)value);

        public static float Abs(float value) => Math.Abs(value);

        public static int Abs(int value) => Math.Abs(value);

        public static Vector3 Abs(Vector3 value) => new Vector3(Math.Abs(value.X), Math.Abs(value.Y), Math.Abs(value.Z));

        public static float Lerp(float p1, float p2, float t) {
            if ((double)t < 0.0)
                return p1;
            return (double)t > 1.0 ? p2 : p1 + (p2 - p1) * t;
        }

        public static Vector3 Lerp(Vector3 p1, Vector3 p2, float t) {
            if ((double)t < 0.0)
                return p1;
            return (double)t > 1.0 ? p2 : p1 + (p2 - p1) * t;
        }

        public static float Modulo(float a, float b) => a - b * (float)Math.Floor((double)a / (double)b);

        public static float Distance(float p1, float p2) => Mathf.Abs(p1 - p2);
    }
}
