#pragma once
#ifdef SMath_Vector3_EXPORTS
#define SMath_Vector3 __declspec(dllexport)
#else
#define SMath_Vector3 __declspec(dllimport)
#endif

#include <stdio.h>
#include <cmath>
#include <iostream>

namespace Stulu {

    class Vector3 {

    private:

    public:

        float x;
        float y;
        float z;


        SMath_Vector3 Vector3();


        SMath_Vector3 Vector3(float uX, float uY, float uZ);

        SMath_Vector3 ~Vector3();


        SMath_Vector3 Vector3(const Vector3& v);

        SMath_Vector3 Vector3& operator=(const Vector3& v);



        SMath_Vector3 void operator+=(const Vector3& v);

        SMath_Vector3 Vector3 operator+(const Vector3& v)const;


        SMath_Vector3 void operator-=(const Vector3& v);

        SMath_Vector3 Vector3 operator-(const Vector3& v)const;


        SMath_Vector3 void operator*=(const float s);

        SMath_Vector3 Vector3 operator*(const float s)const;



        SMath_Vector3 void operator /=(const float s);

        SMath_Vector3 Vector3 operator/(const float s)const;


        SMath_Vector3 float operator*(const Vector3& v) const;

        SMath_Vector3 float dot(const Vector3& v) const;

        SMath_Vector3 float angle(const Vector3& v);


        SMath_Vector3 void operator %=(const Vector3& v);

        SMath_Vector3 Vector3 operator %(const Vector3& v) const;

        SMath_Vector3 Vector3 cross(const Vector3& v) const;

        SMath_Vector3 void conjugate();

        SMath_Vector3 void normalize();

        SMath_Vector3 float magnitude();

        SMath_Vector3 float magnitudeSquare();


        SMath_Vector3 void zero();

        SMath_Vector3 void absolute();

        SMath_Vector3 Vector3 rotateVectorAboutAngleAndAxis(float uAngle, Vector3& uAxis);

        SMath_Vector3 void show();

        SMath_Vector3 void negate();


    };

}
