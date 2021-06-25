#pragma once
#ifdef SMath_Matrix3_EXPORTS
#define SMath_Matrix3 __declspec(dllexport)
#else
#define SMath_Matrix3 __declspec(dllimport)
#endif
#include <stdio.h>
#include <iostream>
#include "Vector3.h"

namespace Stulu {

    class Matrix3 {

    private:

    public:

        float matrixData[9] = { 0.0 };


        SMath_Matrix3 Matrix3();

        SMath_Matrix3 Matrix3(float m0, float m3, float m6, float m1, float m4, float m7, float m2, float m5, float m8);

        SMath_Matrix3 Matrix3& operator=(const Matrix3& value);

        SMath_Matrix3 ~Matrix3();

        SMath_Matrix3 Matrix3 operator+(const Matrix3& m) const;

        SMath_Matrix3 void operator+=(const Matrix3& m);

        SMath_Matrix3 Matrix3 operator*(const float s) const;

        SMath_Matrix3 void operator*=(const float s);


        SMath_Matrix3 Vector3 operator*(const Vector3& v) const;

        SMath_Matrix3 Vector3 transformVectorByMatrix(const Vector3& v) const;

        SMath_Matrix3 Matrix3 operator*(const Matrix3& m) const;

        SMath_Matrix3 void operator*=(const Matrix3& m);

        SMath_Matrix3 void setMatrixAsIdentityMatrix();


        SMath_Matrix3 void setMatrixAsInverseOfGivenMatrix(const Matrix3& m);

        SMath_Matrix3 Matrix3 getInverseOfMatrix() const;

        SMath_Matrix3 void invertMatrix();

        SMath_Matrix3 float getMatrixDeterminant() const;

        SMath_Matrix3 void setMatrixAsTransposeOfGivenMatrix(const Matrix3& m);

        SMath_Matrix3 Matrix3 getTransposeOfMatrix() const;


        SMath_Matrix3 void invertAndTransposeMatrix();


        SMath_Matrix3 void makeRotationMatrixAboutXAxisByAngle(float uAngle);

        SMath_Matrix3 void makeRotationMatrixAboutYAxisByAngle(float uAngle);

        SMath_Matrix3 void makeRotationMatrixAboutZAxisByAngle(float uAngle);


        SMath_Matrix3 void transformMatrixAboutXAxisByAngle(float uAngle);

        SMath_Matrix3 void transformMatrixAboutYAxisByAngle(float uAngle);

        SMath_Matrix3 void transformMatrixAboutZAxisByAngle(float uAngle);


        SMath_Matrix3 void show();

    };

} 