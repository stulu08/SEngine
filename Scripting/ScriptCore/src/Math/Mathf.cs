using System;

namespace Stulu {
    /// <summary>
    /// Contains Floating Point math functions
    /// </summary>
	public static class Mathf {
        /// <summary>
        /// Just 0
        /// </summary>
        public const float zero = 0;
        /// <summary>
        /// Represents the natural logarithmic base, specified by the constant, e
        /// </summary>
        public const float E = 2.718281828459045f;
        /// <summary>
        /// Represents the ratio of the circumference of a circle to its diameter, specified by the constant, π
        /// </summary>
        public const float PI = 3.141592653f;
        /// <summary>
        /// Represents π divided by 2
        /// </summary>
        public const float PI_2 = 1.5707963265f;
        /// <summary>
        /// Represents π multiplied by 2
        /// </summary>
        public const float TwoPI = 6.283185306f;
        /// <summary>
        /// A tiny floating point value
        /// </summary>
        public const float Epsilon = 1E-05f;
        /// <summary>
        /// Degrees-to-radians conversion constant, equal to (PI * 2) / 360.
        /// </summary>
        public const float Deg2Rad = 0.01745329251f;
        /// <summary>
        /// Radians-to-degrees conversion constant, equal to 360 / (PI * 2).
        /// </summary>
        public const float Rad2Deg = 57.2957795131f;
        /// <summary>
        /// Represents the smallest floating point value
        /// </summary>
        public const float MinValue = float.MinValue;
        /// <summary>
        /// Represents the biggest floating point value
        /// </summary>
        public const float MaxValue = float.MaxValue;

        /// <summary>
        /// Converts degrees to radians
        /// </summary>
        public static float Radians(float degrees) => degrees * Deg2Rad;
        /// <summary>
        /// Converts degrees to radians
        /// </summary>
        public static Vector2 Radians(Vector2 degrees) => new Vector2(degrees.x * Deg2Rad, degrees.y * Deg2Rad);
        /// <summary>
        /// Converts degrees to radians
        /// </summary>
        public static Vector3 Radians(Vector3 degrees) => new Vector3(degrees.x * Deg2Rad, degrees.y * Deg2Rad, degrees.z * Deg2Rad);
        /// <summary>
        /// Converts degrees to radians
        /// </summary>
        public static Vector4 Radians(Vector4 degrees) => new Vector4(degrees.x * Deg2Rad, degrees.y * Deg2Rad, degrees.z * Deg2Rad, degrees.w * Deg2Rad);
        /// <summary>
        /// Converts radians to degrees
        /// </summary>
        public static float Degrees(float radians) => radians * Rad2Deg;
        /// <summary>
        /// Converts radians to degrees
        /// </summary>
        public static Vector2 Degrees(Vector2 radians) => new Vector2(radians.x * Rad2Deg, radians.y * Rad2Deg);
        /// <summary>
        /// Converts radians to degrees
        /// </summary>
        public static Vector3 Degrees(Vector3 radians) => new Vector3(radians.x * Rad2Deg, radians.y * Rad2Deg, radians.z * Rad2Deg);
        /// <summary>
        /// Converts radians to degrees
        /// </summary>
        public static Vector4 Degrees(Vector4 radians) => new Vector4(radians.x * Rad2Deg, radians.y * Rad2Deg, radians.z * Rad2Deg, radians.w * Rad2Deg);
        /// <summary>
        /// Returns sine of the angle
        /// </summary>
        public static float Sin(float angle) => (float)Math.Sin((double)angle);
        /// <summary>
        /// Returns cosine of the angle
        /// </summary>
        public static float Cos(float angle) => (float)Math.Cos((double)angle);
        /// <summary>
        /// Returns tangent of the angle
        /// </summary>
        public static float Tan(float angle) => (float)Math.Tan((double)angle);
        /// <summary>
        /// Returns the angle whose sine is the specified number
        /// </summary>
        public static float Asin(float x) => (float)Math.Asin((double)x);
        /// <summary>
        /// Returns the angle whose cosine is the specified number
        /// </summary>
        public static float Acos(float x) => (float)Math.Acos((double)x);
        /// <summary>
        /// Returns the angle whose tangent is the specified number
        /// </summary>
        public static float Atan(float x) => (float)Math.Atan((double)x);
        /// <summary>
        /// Returns the angle whose tangent is the quotient of two specified number
        /// </summary>
        public static float Atan2(float y, float x) => (float)Math.Atan2((double)y, (double)x);
        /// <summary>
        /// Returns value clamped to the inclusive range of min and max
        /// </summary>
        public static float Clamp(float value, float min, float max) {
            if ((double)value < (double)min)
                return min;
            return (double)value > (double)max ? max : value;
        }
        /// <summary>
        /// Returns the number, which is smaller
        /// </summary>
        public static float Min(float v0, float v1) => (double)v0 >= (double)v1 ? v1 : v0;
        /// <summary>
        /// Returns the number, which is bigger
        /// </summary>
        public static float Max(float v0, float v1) => (double)v0 <= (double)v1 ? v1 : v0;
        /// <summary>
        /// Returns the largest integral value less than or equal to x
        /// </summary>
        public static float Floor(float x) => (float)Math.Floor((double)x);
        /// <summary>
        /// Returns the smallest integral value less than or equal to x
        /// </summary>
        public static float Ceil(float x) => (float)Math.Ceiling((double)x);
        /// <summary>
        /// Rounds a decimal value to a specified number of fractional digits
        /// </summary>
        public static float Round(float value, int decimals = 8) {
            return ((float)Mathf.Floor(value * Mathf.Pow(10, decimals) + 0.5f)) / Mathf.Pow(10, decimals);
        }
        /// <summary>
        /// Return the square root of x
        /// </summary>
        public static float Sqrt(float x) {
            float sqrt = x / 2;
            float temp = 0;
            while (sqrt != temp) {
                temp = sqrt;
                sqrt = (x / temp + temp) / 2;
            }
            return sqrt;
        }
        /// <summary>
        /// Returns E raised to x
        /// </summary>
        public static float Exp(float x) => Mathf.Pow(E, x);
        /// <summary>
        /// Returns x raised to y
        /// </summary>
        public static float Pow(float x, float y) => (float)Math.Pow((double)x, (double)y);
        /// <summary>
        /// Returns the logarithm of x
        /// </summary>
        public static float Log(float x) => (float)Math.Log((double)x);
        /// <summary>
        /// Returns the base 10 logarithm of x
        /// </summary>
        public static float Log10(float x) => (float)Math.Log10((double)x);
        /// <summary>
        /// Returns the logarithm of x with a specified base.
        /// </summary>
        public static float LogBase(float x, float newBase) => Mathf.Log(x) / Mathf.Log(newBase);
        /// <summary>
        /// Returns the absolute value of x
        /// </summary>
        public static float Abs(float x) => Math.Abs(x);
        /// <summary>
        /// Returns the absolute value of x
        /// </summary>
        public static int Abs(int x) => Math.Abs(x);
        /// <summary>
        /// Returns the absolute value of x
        /// </summary>
        public static Vector3 Abs(Vector3 x) => new Vector3(Math.Abs(x.X), Math.Abs(x.Y), Math.Abs(x.Z));
        /// <summary>
        /// Return the linear interpolation, equals to a + (b - a) * t.
        /// </summary>
        public static float Lerp(float a, float b, float t) {
            if ((double)t < 0.0)
                return a;
            return (double)t > 1.0 ? b : a + (b - a) * t;
        }
        /// <summary>
        /// Return the linear interpolation, equals to a + (b - a) * t.
        /// </summary>
        public static Vector3 Lerp(Vector3 a, Vector3 b, float t) {
            if ((double)t < 0.0)
                return a;
            return (double)t > 1.0 ? b : a + (b - a) * t;
        }
    }
}
