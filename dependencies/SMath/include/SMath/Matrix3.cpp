#include "Matrix3.h"
#include "Constants.h"

namespace Stulu {



    Matrix3::Matrix3() {


        for (int i = 0; i < 9; i++) {
            matrixData[i] = 0.0f;
        }

        matrixData[0] = matrixData[4] = matrixData[8] = 1.0f;

    };


    Matrix3::Matrix3(float m0, float m3, float m6, float m1, float m4, float m7, float m2, float m5, float m8) {

        matrixData[0] = m0;
        matrixData[3] = m3;
        matrixData[6] = m6;

        matrixData[1] = m1;
        matrixData[4] = m4;
        matrixData[7] = m7;

        matrixData[2] = m2;
        matrixData[5] = m5;
        matrixData[8] = m8;

    };

    Matrix3& Matrix3::operator=(const Matrix3& value) {

        for (int i = 0; i < 9; i++) {
            matrixData[i] = value.matrixData[i];
        }

        return *this;
    };


    Matrix3::~Matrix3() {

    }


    Matrix3 Matrix3::operator+(const Matrix3& m) const {

        Matrix3 n;

        n.matrixData[0] = matrixData[0] + m.matrixData[0];
        n.matrixData[3] = matrixData[3] + m.matrixData[3];
        n.matrixData[6] = matrixData[6] + m.matrixData[6];

        n.matrixData[1] = matrixData[1] + m.matrixData[1];
        n.matrixData[4] = matrixData[4] + m.matrixData[4];
        n.matrixData[7] = matrixData[7] + m.matrixData[7];

        n.matrixData[2] = matrixData[2] + m.matrixData[2];
        n.matrixData[5] = matrixData[5] + m.matrixData[5];
        n.matrixData[8] = matrixData[8] + m.matrixData[8];

        return n;
    }

    void Matrix3::operator+=(const Matrix3& m) {

        matrixData[0] += m.matrixData[0];
        matrixData[3] += m.matrixData[3];
        matrixData[6] += m.matrixData[6];

        matrixData[1] += m.matrixData[1];
        matrixData[4] += m.matrixData[4];
        matrixData[7] += m.matrixData[7];

        matrixData[2] += m.matrixData[2];
        matrixData[5] += m.matrixData[5];
        matrixData[8] += m.matrixData[8];

    }

    Matrix3 Matrix3::operator*(const float s) const {

        Matrix3 n;

        n.matrixData[0] = matrixData[0] * s;
        n.matrixData[3] = matrixData[3] * s;
        n.matrixData[6] = matrixData[6] * s;

        n.matrixData[1] = matrixData[1] * s;
        n.matrixData[4] = matrixData[4] * s;
        n.matrixData[7] = matrixData[7] * s;

        n.matrixData[2] = matrixData[2] * s;
        n.matrixData[5] = matrixData[5] * s;
        n.matrixData[8] = matrixData[8] * s;

        return n;

    }

    void Matrix3::operator*=(const float s) {

        matrixData[0] *= s;
        matrixData[3] *= s;
        matrixData[6] *= s;

        matrixData[1] *= s;
        matrixData[4] *= s;
        matrixData[7] *= s;

        matrixData[2] *= s;
        matrixData[5] *= s;
        matrixData[8] *= s;

    }



    Vector3 Matrix3::operator*(const Vector3& v) const {

        return Vector3(matrixData[0] * v.x + matrixData[3] * v.y + matrixData[6] * v.z,
            matrixData[1] * v.x + matrixData[4] * v.y + matrixData[7] * v.z,
            matrixData[2] * v.x + matrixData[5] * v.y + matrixData[8] * v.z);

    }


    Vector3 Matrix3::transformVectorByMatrix(const Vector3& v) const {

        return (*this) * v;
    }


    Matrix3 Matrix3::operator*(const Matrix3& m) const {



        return Matrix3(matrixData[0] * m.matrixData[0] + matrixData[3] * m.matrixData[1] + matrixData[6] * m.matrixData[2],
            matrixData[0] * m.matrixData[3] + matrixData[3] * m.matrixData[4] + matrixData[6] * m.matrixData[5],
            matrixData[0] * m.matrixData[6] + matrixData[3] * m.matrixData[7] + matrixData[6] * m.matrixData[8],

            matrixData[1] * m.matrixData[0] + matrixData[4] * m.matrixData[1] + matrixData[7] * m.matrixData[2],
            matrixData[1] * m.matrixData[3] + matrixData[4] * m.matrixData[4] + matrixData[7] * m.matrixData[5],
            matrixData[1] * m.matrixData[6] + matrixData[4] * m.matrixData[7] + matrixData[7] * m.matrixData[8],

            matrixData[2] * m.matrixData[0] + matrixData[5] * m.matrixData[1] + matrixData[8] * m.matrixData[2],
            matrixData[2] * m.matrixData[3] + matrixData[5] * m.matrixData[4] + matrixData[8] * m.matrixData[5],
            matrixData[2] * m.matrixData[6] + matrixData[5] * m.matrixData[7] + matrixData[8] * m.matrixData[8]);

    }


    void Matrix3::operator*=(const Matrix3& m) {

        float t1;
        float t2;
        float t3;

        t1 = matrixData[0] * m.matrixData[0] + matrixData[3] * m.matrixData[1] + matrixData[6] * m.matrixData[2];
        t2 = matrixData[0] * m.matrixData[3] + matrixData[3] * m.matrixData[4] + matrixData[6] * m.matrixData[5];
        t3 = matrixData[0] * m.matrixData[6] + matrixData[3] * m.matrixData[7] + matrixData[6] * m.matrixData[8];

        matrixData[0] = t1;
        matrixData[3] = t2;
        matrixData[6] = t3;

        t1 = matrixData[1] * m.matrixData[0] + matrixData[4] * m.matrixData[1] + matrixData[7] * m.matrixData[2];
        t2 = matrixData[1] * m.matrixData[3] + matrixData[4] * m.matrixData[4] + matrixData[7] * m.matrixData[5];
        t3 = matrixData[1] * m.matrixData[6] + matrixData[4] * m.matrixData[7] + matrixData[7] * m.matrixData[8];

        matrixData[1] = t1;
        matrixData[4] = t2;
        matrixData[7] = t3;


        t1 = matrixData[2] * m.matrixData[0] + matrixData[5] * m.matrixData[1] + matrixData[8] * m.matrixData[2];
        t2 = matrixData[2] * m.matrixData[3] + matrixData[5] * m.matrixData[4] + matrixData[8] * m.matrixData[5];
        t3 = matrixData[2] * m.matrixData[6] + matrixData[5] * m.matrixData[7] + matrixData[8] * m.matrixData[8];

        matrixData[2] = t1;
        matrixData[5] = t2;
        matrixData[8] = t3;

    }


    void Matrix3::setMatrixAsIdentityMatrix() {

        for (int i = 0; i < 9; i++) {
            matrixData[i] = 0.0f;
        }

        matrixData[0] = matrixData[4] = matrixData[8] = 1.0f;

    }



    void Matrix3::setMatrixAsInverseOfGivenMatrix(const Matrix3& m) {


        float t1 = m.matrixData[0] * m.matrixData[4];
        float t2 = m.matrixData[0] * m.matrixData[7];
        float t3 = m.matrixData[3] * m.matrixData[1];
        float t4 = m.matrixData[6] * m.matrixData[1];
        float t5 = m.matrixData[3] * m.matrixData[2];
        float t6 = m.matrixData[6] * m.matrixData[2];

        float det = (t1 * m.matrixData[8] - t2 * m.matrixData[5] - t3 * m.matrixData[8] + t4 * m.matrixData[5] + t5 * m.matrixData[7] - t6 * m.matrixData[4]);

        if (det == 0.0) return;

        float invd = 1.0f / det;

        float m0 = (m.matrixData[4] * m.matrixData[8] - m.matrixData[7] * m.matrixData[5]) * invd;
        float m3 = -(m.matrixData[3] * m.matrixData[8] - m.matrixData[6] * m.matrixData[5]) * invd;

        float m6 = (m.matrixData[3] * m.matrixData[7] - m.matrixData[6] * m.matrixData[4]) * invd;

        float m1 = -(m.matrixData[1] * m.matrixData[8] - m.matrixData[7] * m.matrixData[2]) * invd;

        float m4 = (m.matrixData[0] * m.matrixData[8] - t6) * invd;

        float m7 = -(t2 - t4) * invd;

        float m2 = (m.matrixData[1] * m.matrixData[5] - m.matrixData[4] * m.matrixData[2]) * invd;

        float m5 = -(m.matrixData[0] * m.matrixData[5] - t5) * invd;

        float m8 = (t1 - t3) * invd;

        matrixData[0] = m0;
        matrixData[3] = m3;
        matrixData[6] = m6;

        matrixData[1] = m1;
        matrixData[4] = m4;
        matrixData[7] = m7;

        matrixData[2] = m2;
        matrixData[5] = m5;
        matrixData[8] = m8;
    }


    Matrix3 Matrix3::getInverseOfMatrix() const {

        Matrix3 result;
        result.setMatrixAsInverseOfGivenMatrix(*this);
        return result;
    }


    void Matrix3::invertMatrix() {

        setMatrixAsInverseOfGivenMatrix(*this);
    }


    float Matrix3::getMatrixDeterminant() const {

        float t1 = matrixData[0] * matrixData[4];
        float t2 = matrixData[0] * matrixData[7];
        float t3 = matrixData[3] * matrixData[1];
        float t4 = matrixData[6] * matrixData[1];
        float t5 = matrixData[3] * matrixData[2];
        float t6 = matrixData[6] * matrixData[2];

        float det = (t1 * matrixData[8] - t2 * matrixData[5] - t3 * matrixData[8] + t4 * matrixData[5] + t5 * matrixData[7] - t6 * matrixData[4]);

        return det;
    }


    void Matrix3::setMatrixAsTransposeOfGivenMatrix(const Matrix3& m) {

        matrixData[0] = m.matrixData[0];
        matrixData[3] = m.matrixData[1];
        matrixData[6] = m.matrixData[2];

        matrixData[1] = m.matrixData[3];
        matrixData[4] = m.matrixData[4];
        matrixData[7] = m.matrixData[5];

        matrixData[2] = m.matrixData[6];
        matrixData[5] = m.matrixData[7];
        matrixData[8] = m.matrixData[8];

    }


    Matrix3 Matrix3::getTransposeOfMatrix() const {

        Matrix3 result;
        result.setMatrixAsTransposeOfGivenMatrix(*this);
        return result;

    }



    void Matrix3::invertAndTransposeMatrix() {

        float det = matrixData[0] * (matrixData[4] * matrixData[8] - matrixData[5] * matrixData[7]) - matrixData[3] * (matrixData[1] * matrixData[8] - matrixData[2] * matrixData[7]) + matrixData[6] * (matrixData[1] * matrixData[5] - matrixData[2] * matrixData[4]);

        Matrix3 transpose;

        transpose.matrixData[0] = matrixData[0];
        transpose.matrixData[1] = matrixData[3];
        transpose.matrixData[2] = matrixData[6];

        transpose.matrixData[3] = matrixData[1];
        transpose.matrixData[4] = matrixData[4];
        transpose.matrixData[5] = matrixData[7];

        transpose.matrixData[6] = matrixData[2];
        transpose.matrixData[7] = matrixData[5];
        transpose.matrixData[8] = matrixData[8];

        float m11, m12, m13, m21, m22, m23, m31, m32, m33;

        m11 = +(transpose.matrixData[4] * transpose.matrixData[8] - transpose.matrixData[5] * transpose.matrixData[7]) / det;
        m12 = -(transpose.matrixData[1] * transpose.matrixData[8] - transpose.matrixData[2] * transpose.matrixData[7]) / det;
        m13 = +(transpose.matrixData[1] * transpose.matrixData[5] - transpose.matrixData[2] * transpose.matrixData[4]) / det;

        m21 = -(transpose.matrixData[3] * transpose.matrixData[8] - transpose.matrixData[5] * transpose.matrixData[6]) / det;
        m22 = +(transpose.matrixData[0] * transpose.matrixData[8] - transpose.matrixData[2] * transpose.matrixData[6]) / det;
        m23 = -(transpose.matrixData[0] * transpose.matrixData[5] - transpose.matrixData[2] * transpose.matrixData[3]) / det;

        m31 = +(transpose.matrixData[3] * transpose.matrixData[7] - transpose.matrixData[4] * transpose.matrixData[6]) / det;
        m32 = -(transpose.matrixData[0] * transpose.matrixData[7] - transpose.matrixData[1] * transpose.matrixData[6]) / det;
        m33 = +(transpose.matrixData[0] * transpose.matrixData[4] - transpose.matrixData[1] * transpose.matrixData[3]) / det;

        Matrix3 preResult;

        preResult.matrixData[0] = m11;
        preResult.matrixData[1] = m21;
        preResult.matrixData[2] = m31;

        preResult.matrixData[3] = m12;
        preResult.matrixData[4] = m22;
        preResult.matrixData[5] = m32;

        preResult.matrixData[6] = m13;
        preResult.matrixData[7] = m23;
        preResult.matrixData[8] = m33;

        matrixData[0] = preResult.matrixData[0];
        matrixData[1] = preResult.matrixData[3];
        matrixData[2] = preResult.matrixData[6];

        matrixData[3] = preResult.matrixData[1];
        matrixData[4] = preResult.matrixData[4];
        matrixData[5] = preResult.matrixData[7];

        matrixData[6] = preResult.matrixData[2];
        matrixData[7] = preResult.matrixData[5];
        matrixData[8] = preResult.matrixData[8];
    }


    void Matrix3::makeRotationMatrixAboutXAxisByAngle(float uAngle) {

        uAngle = DegreesToRad(uAngle);

        (*this).setMatrixAsIdentityMatrix();

        Matrix3 m(1.0, 0.0, 0.0,
            0.0, cos(uAngle), -sin(uAngle),
            0.0, sin(uAngle), cos(uAngle));

        *this = m * (*this);

    }

    void Matrix3::makeRotationMatrixAboutYAxisByAngle(float uAngle) {

        uAngle = DegreesToRad(uAngle);
        (*this).setMatrixAsIdentityMatrix();

        Matrix3 m(cos(uAngle), 0.0, sin(uAngle),
            0.0, 1.0, 0.0,
            -sin(uAngle), 0.0, cos(uAngle));

        *this = m * (*this);

    }

    void Matrix3::makeRotationMatrixAboutZAxisByAngle(float uAngle) {

        uAngle = DegreesToRad(uAngle);
        (*this).setMatrixAsIdentityMatrix();

        Matrix3 m(cos(uAngle), -sin(uAngle), 0.0,
            sin(uAngle), cos(uAngle), 0.0,
            0.0, 0.0, 1.0);

        *this = m * (*this);
    }


    void Matrix3::transformMatrixAboutXAxisByAngle(float uAngle) {

        uAngle = DegreesToRad(uAngle);

        Matrix3 m(1.0, 0.0, 0.0,
            0.0, cos(uAngle), -sin(uAngle),
            0.0, sin(uAngle), cos(uAngle));

        *this = m * (*this);

    }

    void Matrix3::transformMatrixAboutYAxisByAngle(float uAngle) {

        uAngle = DegreesToRad(uAngle);

        Matrix3 m(cos(uAngle), 0.0, sin(uAngle),
            0.0, 1.0, 0.0,
            -sin(uAngle), 0.0, cos(uAngle));

        *this = m * (*this);


    }

    void Matrix3::transformMatrixAboutZAxisByAngle(float uAngle) {

        uAngle = DegreesToRad(uAngle);

        Matrix3 m(cos(uAngle), -sin(uAngle), 0.0,
            sin(uAngle), cos(uAngle), 0.0,
            0.0, 0.0, 1.0);

        *this = m * (*this);
    }


    void Matrix3::show() {

        std::cout << "[" << matrixData[0] << "," << matrixData[3] << "," << matrixData[6] << "," << std::endl;
        std::cout << matrixData[1] << "," << matrixData[4] << "," << matrixData[7] << "," << std::endl;
        std::cout << matrixData[2] << "," << matrixData[5] << "," << matrixData[8] << "]" << std::endl;
    }

}