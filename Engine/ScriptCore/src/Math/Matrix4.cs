#pragma warning disable 1591
namespace Stulu {
    public struct Matrix4 {
		public float D00, D10, D20, D30;  // Column 0
		public float D01, D11, D21, D31;  // Column 1
		public float D02, D12, D22, D32;  // Column 2
		public float D03, D13, D23, D33;  // Column 3

		//a b c d
		//e f g h
		//i j k l
		//m n o p

		public float a { get => this.D00; set { this.D00 = value; } }
        public float b { get => this.D01; set { this.D01 = value; } }
        public float c { get => this.D02; set { this.D02 = value; } }
        public float d { get => this.D03; set { this.D03 = value; } }
        public float e { get => this.D10; set { this.D10 = value; } }
        public float f { get => this.D11; set { this.D11 = value; } }
        public float g { get => this.D12; set { this.D12 = value; } }
        public float h { get => this.D13; set { this.D13 = value; } }
        public float i { get => this.D20; set { this.D20 = value; } }
        public float j { get => this.D21; set { this.D21 = value; } }
        public float k { get => this.D22; set { this.D22 = value; } }
        public float l { get => this.D23; set { this.D23 = value; } }
        public float m { get => this.D30; set { this.D30 = value; } }
        public float n { get => this.D31; set { this.D31 = value; } }
        public float o { get => this.D32; set { this.D32 = value; } }
        public float p { get => this.D33; set { this.D33 = value; } }

        public Matrix4(Vector4[] values) {
            this.D00 = values[0].x;
            this.D01 = values[0].y;
            this.D02 = values[0].z;
            this.D03 = values[0].w;

            this.D10 = values[1].x;
            this.D11 = values[1].y;
            this.D12 = values[1].z;
            this.D13 = values[1].w;
                                  
            this.D20 = values[2].x;
            this.D21 = values[2].y;
            this.D22 = values[2].z;;
            this.D23 = values[2].w;
                                  
            this.D30 = values[3].x;
            this.D31 = values[3].y;
            this.D32 = values[3].z;
            this.D33 = values[3].w;
        }
        public Matrix4(Vector4 valueX, Vector4 valueY, Vector4 valueZ, Vector4 valueW) {
            this.D00 = valueX.x;
            this.D01 = valueX.y;
            this.D02 = valueX.z;
            this.D03 = valueX.w;

            this.D10 = valueY.x;
            this.D11 = valueY.y;
            this.D12 = valueY.z;
            this.D13 = valueY.w;

            this.D20 = valueZ.x;
            this.D21 = valueZ.y;
            this.D22 = valueZ.z; ;
            this.D23 = valueZ.w;

            this.D30 = valueW.x;
            this.D31 = valueW.y;
            this.D32 = valueW.z;
            this.D33 = valueW.w;
        }
        public Matrix4(float value) {
            this.D00 = value; this.D01 = 0.0f; this.D02 = 0.0f; this.D03 = 0.0f;
            this.D10 = 0.0f; this.D11 = value; this.D12 = 0.0f; this.D13 = 0.0f;
            this.D20 = 0.0f; this.D21 = 0.0f; this.D22 = value; this.D23 = 0.0f;
            this.D30 = 0.0f; this.D31 = 0.0f; this.D32 = 0.0f; this.D33 = value;
        }
        public Vector3 Translation {
            get => new Vector3(this.D03, this.D13, this.D23);
            set {
                this.D03 = value.X;
                this.D13 = value.Y;
                this.D23 = value.Z;
            }
        }

        public static Matrix4 Translate(Vector3 translation) => new Matrix4(1f) {
            D03 = translation.X,
            D13 = translation.Y,
            D23 = translation.Z
        };
        public static Matrix4 Rotate(Quaternion rotation) {
            float q0 = rotation.w;
            float q1 = rotation.x;
            float q2 = rotation.y;
            float q3 = rotation.z;

            float D00 = 2 * (q0 * q0 + q1 * q1) - 1;
            float D01 = 2 * (q1 * q2 - q0 * q3);
            float D02 = 2 * (q1 * q3 + q0 * q2);

            float D10 = 2 * (q1 * q2 + q0 * q3);
            float D11 = 2 * (q0 * q0 + q2 * q2) - 1;
            float D12 = 2 * (q2 * q3 - q0 * q1);

            float D20 = 2 * (q1 * q3 - q0 * q2);
            float D21 = 2 * (q2 * q3 + q0 * q1);
            float D22 = 2 * (q0 * q0 + q3 * q3) - 1;

            return new Matrix4(new Vector4[4] {
                new Vector4(D00,D01,D02,0),
                new Vector4(D10,D11,D12,0),
                new Vector4(D20,D21,D22,0),
                new Vector4(0,0,0,1),
            });
        }
        public static Matrix4 Inverse(Matrix4 m) {
            //https://github.com/g-truc/glm/blob/cc98465e3508535ba8c7f6208df934c156a018dc/glm/detail/func_matrix.inl#L347
            float Coef00 = m[2][2] * m[3][3] - m[3][2] * m[2][3];
            float Coef02 = m[1][2] * m[3][3] - m[3][2] * m[1][3];
            float Coef03 = m[1][2] * m[2][3] - m[2][2] * m[1][3];

            float Coef04 = m[2][1] * m[3][3] - m[3][1] * m[2][3];
            float Coef06 = m[1][1] * m[3][3] - m[3][1] * m[1][3];
            float Coef07 = m[1][1] * m[2][3] - m[2][1] * m[1][3];

            float Coef08 = m[2][1] * m[3][2] - m[3][1] * m[2][2];
            float Coef10 = m[1][1] * m[3][2] - m[3][1] * m[1][2];
            float Coef11 = m[1][1] * m[2][2] - m[2][1] * m[1][2];

            float Coef12 = m[2][0] * m[3][3] - m[3][0] * m[2][3];
            float Coef14 = m[1][0] * m[3][3] - m[3][0] * m[1][3];
            float Coef15 = m[1][0] * m[2][3] - m[2][0] * m[1][3];

            float Coef16 = m[2][0] * m[3][2] - m[3][0] * m[2][2];
            float Coef18 = m[1][0] * m[3][2] - m[3][0] * m[1][2];
            float Coef19 = m[1][0] * m[2][2] - m[2][0] * m[1][2];

            float Coef20 = m[2][0] * m[3][1] - m[3][0] * m[2][1];
            float Coef22 = m[1][0] * m[3][1] - m[3][0] * m[1][1];
            float Coef23 = m[1][0] * m[2][1] - m[2][0] * m[1][1];

            Vector4 Fac0 = new Vector4(Coef00, Coef00, Coef02, Coef03);
            Vector4 Fac1 = new Vector4(Coef04, Coef04, Coef06, Coef07);
            Vector4 Fac2 = new Vector4(Coef08, Coef08, Coef10, Coef11);
            Vector4 Fac3 = new Vector4(Coef12, Coef12, Coef14, Coef15);
            Vector4 Fac4 = new Vector4(Coef16, Coef16, Coef18, Coef19);
            Vector4 Fac5 = new Vector4(Coef20, Coef20, Coef22, Coef23);

            Vector4 Vec0 = new Vector4(m[1][0], m[0][0], m[0][0], m[0][0]);
            Vector4 Vec1 = new Vector4(m[1][1], m[0][1], m[0][1], m[0][1]);
            Vector4 Vec2 = new Vector4(m[1][2], m[0][2], m[0][2], m[0][2]);
            Vector4 Vec3 = new Vector4(m[1][3], m[0][3], m[0][3], m[0][3]);

            Vector4 Inv0 = Vec1 * Fac0 - Vec2 * Fac1 + Vec3 * Fac2;
            Vector4 Inv1 = Vec0 * Fac0 - Vec2 * Fac3 + Vec3 * Fac4;
            Vector4 Inv2 = Vec0 * Fac1 - Vec1 * Fac3 + Vec3 * Fac5;
            Vector4 Inv3 = Vec0 * Fac2 - Vec1 * Fac4 + Vec2 * Fac5;

            Vector4 SignA = new Vector4(+1, -1, +1, -1);
            Vector4 SignB = new Vector4(-1, +1, -1, +1);
            Matrix4 Inverse = new Matrix4(Inv0 * SignA, Inv1 * SignB, Inv2 * SignA, Inv3 * SignB);

            Vector4 Row0 = new Vector4(Inverse[0][0], Inverse[1][0], Inverse[2][0], Inverse[3][0]);

            Vector4 Dot0 = m[0] * Row0;
            float Dot1 = (Dot0.x + Dot0.y) + (Dot0.z + Dot0.w);

            float OneOverDeterminant = 1f / Dot1;

            return Inverse * OneOverDeterminant;

        }
        public static Matrix4 Ortho(float left, float right, float bottom, float top, float near, float far) => new Matrix4(1f) {
            D00 = 2f / (right - left),
            D11 = 2f / (top - bottom),
            D22 = -2f / (far - near),

            D03 = -(right + left) / (right - left),
            D13 = -(top + bottom) / (top - bottom),
            D23 = -(far + near) / (far - near)
        };
        public static Matrix4 Perspective(float fov, float aspect, float near, float far) => new Matrix4(0f) {
            //https://stackoverflow.com/questions/53245632/general-formula-for-perspective-projection-matrix
            D00 = 1f / (aspect * Mathf.Tan(fov / 2f)),
            D11 = 1f / (Mathf.Tan(fov / 2f)),
            D22 = -(far + near) / (far - near),

            D32 = -1f,
            D23 = -(2f * far * near) / (far - near)
        };

        public static Matrix4 Scale(Vector3 scale) => new Matrix4(1f) {
            D00 = scale.X,
            D11 = scale.Y,
            D22 = scale.Z
        };
        public static Matrix4 Scale(float scale) => new Matrix4(1f) {
            D00 = scale,
            D11 = scale,
            D22 = scale
        };

        public static Matrix4 operator *(Matrix4 left, Matrix4 right) => new Matrix4(0f) {
            a = left.a * right.a + left.b * right.e + left.c * right.i + left.d * right.m,
            b = left.a * right.b + left.b * right.f + left.c * right.j + left.d * right.n,
            c = left.a * right.c + left.b * right.g + left.c * right.k + left.d * right.o,
            d = left.a * right.d + left.b * right.h + left.c * right.l + left.d * right.p,

            e = left.e * right.a + left.f * right.e + left.g * right.i + left.h * right.m,
            f = left.e * right.b + left.f * right.f + left.g * right.j + left.h * right.n,
            g = left.e * right.c + left.f * right.g + left.g * right.k + left.h * right.o,
            h = left.e * right.d + left.f * right.h + left.g * right.l + left.h * right.p,

            i = left.i * right.a + left.j * right.e + left.k * right.i + left.l * right.m,
            j = left.i * right.b + left.j * right.f + left.k * right.j + left.l * right.n,
            k = left.i * right.c + left.j * right.g + left.k * right.k + left.l * right.o,
            l = left.i * right.d + left.j * right.h + left.k * right.l + left.l * right.p,

            m = left.m * right.a + left.n * right.e + left.o * right.i + left.p * right.m,
            n = left.m * right.b + left.n * right.f + left.o * right.j + left.p * right.n,
            o = left.m * right.c + left.n * right.g + left.o * right.k + left.p * right.o,
            p = left.m * right.d + left.n * right.h + left.o * right.l + left.p * right.p,
        };
        public static Matrix4 operator *(Matrix4 left, float right) => new Matrix4(0f) {
            [0] = left[0] * right,
            [1] = left[1] * right,
            [2] = left[2] * right,
            [3] = left[3] * right,
        };

        public Vector4 this[int index] {
            get { switch (index) {
                    case 0:
                        return new Vector4(D00, D01, D02, D03);
                    case 1:
                        return new Vector4(D10, D11, D12, D13);
                    case 2:
                        return new Vector4(D20, D21, D22, D23);
                    case 3:
                        return new Vector4(D30, D31, D32, D33);
                    default:
                        throw new System.IndexOutOfRangeException("Index " + index + " is not in range of Stulu.Matrix4");
                }
            }
            set {
				switch (index) {
                    case 0:
                        D00 = value.x; D01 = value.y; D02 = value.z; D03 = value.w;
                        break;
                    case 1:
                        D10 = value.x; D11 = value.y; D12 = value.z; D13 = value.w;
                        break;
                    case 2:
                        D20 = value.x; D21 = value.y; D22 = value.z; D23 = value.w;
                        break;
                    case 3:
                        D30 = value.x; D31 = value.y; D32 = value.z; D33 = value.w;
                        break;
                    default:
                        throw new System.IndexOutOfRangeException("Index " + index + " is not in range of Stulu.Matrix4");
                }
            }
        }

        public override string ToString() {
            return
          "Matrix4[" + a + "," + b + "," + c + "," + d + "," + "\n " +
                e + "," + f + "," + g + "," + h + "," + "\n " +
                i + "," + j + "," + k + "," + l + "," + "\n " +
                m + "," + b + "," + o + "," + p + "," + "]";
		}

        public static Matrix4 Identy = new Matrix4(1.0f);
	}

}