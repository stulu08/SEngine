#pragma once
#ifdef SMath_Quaternion_EXPORTS
#define SMath_Quaternion __declspec(dllexport)
#else
#define SMath_Quaternion __declspec(dllimport)
#endif
#include <stdio.h>
#include <iostream>
#include "Vector3.h"
#include "Matrix3.h"

namespace Stulu {

    class Quaternion {

    public:

        float s;

        Vector3 v;


        SMath_Quaternion Quaternion(float uS, Vector3& uV);


        SMath_Quaternion ~Quaternion();

        SMath_Quaternion Quaternion(const Quaternion& value);


        SMath_Quaternion Quaternion& operator=(const Quaternion& value);

        SMath_Quaternion void operator+=(const Quaternion& q);


        SMath_Quaternion Quaternion operator+(const Quaternion& q)const;

        SMath_Quaternion void operator-=(const Quaternion& q);


        SMath_Quaternion Quaternion operator-(const Quaternion& q)const;


        SMath_Quaternion void operator*=(const Quaternion& q);


        SMath_Quaternion Quaternion operator*(const Quaternion& q)const;


        SMath_Quaternion Quaternion multiply(const Quaternion& q)const;


        SMath_Quaternion Quaternion operator*(const Vector3& uValue)const;

        SMath_Quaternion float dot(Quaternion& q);

        SMath_Quaternion void operator*=(const float value);

        SMath_Quaternion Quaternion operator*(const float value)const;

        SMath_Quaternion float norm();

        SMath_Quaternion void normalize();

        SMath_Quaternion void convertToUnitNormQuaternion();

        SMath_Quaternion Quaternion conjugate();

        SMath_Quaternion Quaternion inverse();

        SMath_Quaternion void inverse(Quaternion& q);

        SMath_Quaternion Matrix3 transformQuaternionToMatrix3n();

        SMath_Quaternion void transformEulerAnglesToQuaternion(float x, float y, float z);

        SMath_Quaternion Vector3 transformQuaternionToEulerAngles();

        SMath_Quaternion void transformMatrix3nToQuaternion(Matrix3& uMatrix);

        SMath_Quaternion void show();

    };

}